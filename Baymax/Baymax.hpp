#ifndef __BAYMAX_HPP__
#define __BAYMAX_HPP__

#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <cstdio>
#include <unistd.h>
#include <map>
#include <unordered_map>
#include <stdlib.h>
#include <json/json.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "speech.h"
#include "base/http.h"

#define ASR_PATH "./temp_file/asr.wav"
#define CMD_ETC "./command.etc"
#define TTS_PATH "./temp_file/tts.wav"

std::unordered_map<std::string,std::string> key_val;

//所用工具类
class Util
{
	public:
		static bool Exec(std::string cmd,bool is_print) //执行命令
		{
			if(!is_print)
				cmd+=" >/dev/null 2>&1";    //将标准错误的内容重定向到垃圾文件
			FILE *fd=popen(cmd.c_str(),"r");    //调用子进程执行命令
			if(nullptr==fd)
			{
				std::cerr<<"Exec error!"<<std::endl;
				return false;
			}
			char c;
			while(fread(&c,1,1,fd)>0)  //is_print为true时打印所有信息包括空格
				std::cout<<c;
			pclose(fd);
			return true;
		}

		static void Load(std::string path,std::unordered_map<std::string,std::string> &map)//加载文件
		{
			char buf[256];
			std::ifstream in(path.c_str());
			if(!in.is_open())
			{
				std::cerr<<"Open file failed."<<std::endl;   
				exit(1);
			}
			std::string seq=":";  
			while(in.getline(buf,sizeof(buf)))
			{
				std::string str=buf;  //读取的字符串
				size_t index=str.find(seq);   //找符号 :
				if(index==std::string::npos)
				{
					std::cerr<<"Seq not found."<<std::endl;
					continue;
				}
				std::string msg=str.substr(0,index);   //普通用语
				std::string cmd=str.substr(index+seq.size());   //命令
				map[msg]=cmd;
			}
		}

		static void *ThreadRun(void *arg)
		{
			pthread_detach(pthread_self());   //线程分离
			char bar[53]={0};
			const char*lable="|/-\\";
			for(int i=0;i<=50;++i)
			{
				printf("[Recording...][%-51s][%3d%%][%c]\r",bar,i*2,lable[i%4]);
				fflush(stdout);
				bar[i]='=';
				bar[i+1]='>';
				usleep(100000);
			}
			std::cout<<std::endl;
			return (void*) 0;
		}
			
		static void PrintProcess()   //进度条
		{
			pthread_t tid;
			if(pthread_create(&tid,nullptr,ThreadRun,nullptr)!=0)
				std::cerr<<__TIME__<<"Thread creadte failed."<<std::endl;
		}
};

//接入机器人
class Robot
{
	private:
		std::string url;
		std::string apiKey;
		std::string userID;
		aip::HttpClient client;
	private:

		std::string Message2Json(std::string &Wmsg) //将信息转成json串
		{
			Json::Value root;  //相当于一个万能容器，可存放任意类型
			Json::StreamWriterBuilder wb;    //可以向输入流中写文本的对象
			std::ostringstream os;   //输入流的缓冲区

			root["reqType"]=0;  //输入类型为文本
			
			Json::Value item1;
			Json::Value item1_1;
			item1_1["text"]=Wmsg;
			item1["inputText"]=item1_1;
			root["perception"]=item1;  //输入的信息

			Json::Value item2;
			item2["apiKey"]=apiKey;
			item2["userId"]=userID;
			root["userInfo"]=item2;  //用户相关参数

			std::unique_ptr<Json::StreamWriter> sw(wb.newStreamWriter());  //指向对象的智能指针
			sw->write(root,&os);     //向输入流缓冲区中写入root类型的数据
			std::string ret=os.str();  //提取输入流缓冲区内数据
			return ret;   
		}

		std::string RequestRobot(std::string& Jmsg)  //请求机器人
		{
			std::string response;
			//使用HTTP中post方法请求机器人回复(网络服务器)
			int status=client.post(url,nullptr,Jmsg,nullptr,&response); 
			if(status!=CURLcode::CURLE_OK) //请求失败
			{
				std::cerr<<"post error!"<<std::endl;
				return "";
			}
			return response;
		}

		bool IsCodeLegal(int code,std::string& err_msg)   //返回Code 是否合法
		{
			std::unordered_map<int,std::string>err_reason={
				{5000,"无解析结果"},
				{6000,"暂不支持该功能"}};
			auto it=err_reason.find(code);   //在错误原因的集里找返回的code
			if(it!=err_reason.end())    //找到错误的code 就返回false，并输出原因
			{
				err_msg=it->second;
				return false;
			}
			else                 //否则就是合法的code
				return true;
		}

		std::string Json2Message(std::string Rmsg)  //将json串转成信息
		{
			JSONCPP_STRING errs;
			Json::Value root;
			Json::CharReaderBuilder rb;

			std::unique_ptr<Json::CharReader>const cr(rb.newCharReader()); //用于读json串的智能指针
			bool res=cr->parse(Rmsg.data(),Rmsg.data()+Rmsg.size(),&root,&errs);  //用parse解析json串
			if(!res||!errs.empty())  //res为false 或者errs中返回了错误信息，则表示解析失败
			{
				std::cerr<<"pase error!"<<errs<<std::endl;
				return "";
			}
			int code=root["intent"]["code"].asInt();   //找到返回的编码
			std::string errMsg;     

			if(!IsCodeLegal(code,errMsg))    //如果返回的编码不合法则获取错误原因并输出
			{
				std::cerr<<"Response Code Error: "<<errMsg<<std::endl;
				return "";
			}
			std::string ret=root["results"][0]["values"]["text"].asString(); //获取解析出的信息
			return ret;
		}

	public:
		Robot(std::string id="101"):userID(id)
		{
			 url="http://openapi.tuling123.com/openapi/api/v2";
			apiKey="3b5de90c5a8f4f1bbb898e112e640687";
			
		}

		~Robot()
		{}

		std::string Talk(std::string &msg)
		{
			std::string json=Message2Json(msg);        //构造json串
			std::string response=RequestRobot(json);   //请求机器人得到json回复
			std::string echo=Json2Message(response);    //解析得到的json串
			return echo;                                //得到机器人的文本回复
		}

};

//语音转换
class Speech
{
	private:
		std::string appID="16896562";
		std::string apiKey="GwfvLkzG1j5bbEcyaTrzElBl";
		std::string secretKey="EKCN76kWcBYhuKdzmqPx3QSqVFmqgPne";
		aip::Speech *client; 
	public:
	Speech()
	{
		client = new aip::Speech(appID, apiKey, secretKey);    //在语音平台，创建一个自己的客户端(应用)
	}
	~Speech()
	{}

	//语音识别
	bool ASR(std::string& msg)
	{
		std::map<std::string,std::string> options;   //语音识别语言的可选项
		options["dev_pid"] ="1536";
		std::string file_content;         
		aip::get_file_content(ASR_PATH,&file_content);  //将语音文件内容取出

		//调用第三方平台的语音识别接口recognize，将转成的文本文件存在json中
		Json::Value results=client->recognize(file_content,"wav",16000,options); 

		if(results["err_no"].asInt()!=0)   //识别失败输出失败的编码及原因
		{
			std::cerr<<__TIME__<<"Recoginize failed: "<<results["err_no"].asInt()<<" "<<results["err_msg"].asString()<<std::endl;
			return false;
		}
		msg=results["result"][0].asString();   //识别成功获取内容
		return true;
	}

	//语音合成
	void TTS(std::string& msg)
	{
		std::ofstream ofile;     //音频文件
		std::string file_ret;
		std::map<std::string,std::string>options;
		options["spd"]="4";  //语速
		options["pit"]="6";  //语调
		options["vol"]="5";  //音量
		options["per"]="111";   //声音
		options["aue"]="6";    //播放格式

		ofile.open(TTS_PATH,std::ios::out | std::ios::binary);   //创建音频文件以二进制形式打开
		
		//调用第三方平台的语音合成接口text2audio，将msg转成的二进制串存在file_ret中 
		Json::Value result=client->text2audio(msg,options,file_ret);   
		
		if(!file_ret.empty())   //成功合成
				ofile<<file_ret;   //追加重定向，将合成结果返回音频文件
		else   //合成失败输出错误原因
			std::cerr<<__TIME__<<"Text tto audio failed: "<<result["err_msg"]<<std::endl;
		ofile.close();
	}
};

//我的管家
class 	Baymax
{
	private:
		Robot rb;  //机器人
		Speech sh;   //语音转换
		std::unordered_map<std::string,std::string>msg_cmd;  //语音和命令的对应关系

	private:
		bool Record()    //录音
		{
			std::string cmd="arecord -t wav -c 1 -r 16000 -d 5 -f S16_LE ";
			cmd += ASR_PATH;
			if(!Util::Exec(cmd,false))   //执行命令失败
			{
				std::cerr<<__TIME__<<"Record error."<<std::endl;
				return false;
			}
			return true;
		}

		bool Play()   //播放
		{
			std::string cmd="cvlc --play-and-exit ";  //播完就退出
			cmd+=TTS_PATH;
			if(!Util::Exec(cmd,false))
			{
				std::cerr<<__TIME__<<"Play error."<<std::endl;
				return false;
			}
			return true;
		}

		bool Is_cmd(const std::string &recg_msg,std::string& cmd)   //判断人工输入是否命令
		{
			auto it=msg_cmd.find(recg_msg);  //在自定义命令单中找对应的命令
			if(it==msg_cmd.end())  
				return false;
			else  //找到则返回命令
			{
				cmd=it->second;
				return true;
			}
		}
	public:
		Baymax()
		{
			Util::Load(CMD_ETC,msg_cmd);  //加载自定义命令单
		}
		~Baymax()
		{}
		int Run()
		{
			int time=0;
			volatile bool quit=false;
			while(!quit)
			{
			if(time==0)
			{
				std::string ret="来和我聊天吧。";
				sh.TTS(ret);
				Play();
				time++;
				continue;
			}
			Util::PrintProcess();  //打印进度条
			std::string recg_msg;
			if(!Record()||!sh.ASR(recg_msg))    //录音或者识别失败直接进入下次   
			{
				std::cout<<"Recording Again!"<<std::endl;
				continue;
			}
			std::cout<<"我: "<<recg_msg<<std::endl;   //输出语音识别内容
			if(recg_msg=="你走吧。"&& time!=0)
			{
				if(time==1)
				{
					std::string ret1="真的我不要了吗？";
					sh.TTS(ret1);
					std::cout<<"Baymax: "<<ret1<<std::endl;
					Play();
					time++;
					continue;
				}
				if(time==2)
				{
					std::string ret2="再考虑一下吧~";
					sh.TTS(ret2);
					std::cout<<"Baymax: "<<ret2<<std::endl;
				    Play();
					time++;
					continue;
				}
					std::string ret="那我走啦，不要想我哦。";
					sh.TTS(ret);
					std::cout<<"Baymax: "<<ret<<std::endl;
					Play();
					quit=true;
					continue;
			}
			std::string cmd;
			if(Is_cmd(recg_msg,cmd))  //识别是命令
			{
				std::cout<<"[Baymax@localhost]$ "<<cmd<<std::endl;  //打印并执行命令
				if(!Util::Exec(cmd,true))
					return 3;  //命令执行失败  3
			}   
			else    //非命令交给机器人
			{

				std::string msg=rb.Talk(recg_msg);   //识别内容交给机器人回复
				sh.TTS(msg);                   //将机器人回复内容语音合成
				std::cout<<"Baymax: "<<msg<<std::endl;   //打印机器人回复内容
				Play();                                  //播放回复的语音
			}
			}
			return 0;
		}
};
#endif  //_BAYMAX_HPP_

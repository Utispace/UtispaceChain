

#pragma once

#include <libdevcore/Common.h>
#include <libcvvmcore/Instruction.h>
#include <libdevcore/CommonIO.h>
#include <libdevcore/Guards.h>
#include <libdevcore/easylog.h>
#include <boost/timer.hpp>
#include <libdevcore/FileSystem.h>
#include <thread>

using namespace dev::eth;
using namespace std;

namespace dev
{
namespace eth
{



using Address = h160;

struct OpcodeInfo
{
    OpcodeInfo(size_t i,Instruction o=Instruction::STOP,std::string n="INVALID_INSTRUCTION"):index(i),op(o),opname(n),hint(0){}

    size_t index;          //�±�
    Instruction op;     //opcode   
    std::string opname; 
    u64 hint;
};

struct StatItem
{
    StatItem(bytes cspace)
    {
        codespace=cspace;
        code=codespace.data();

        //��ʼ��       
        size_t codesize=codespace.size();

        for (size_t i = 0; i < codesize; ++i)
        {
            Instruction op = Instruction(code[i]);
            OpcodeInfo opcodeinfo(i,op,instructionInfo(op).name);

            //��������
            if (op == Instruction::PUSHC ||
		        op == Instruction::JUMPC ||
		        op == Instruction::JUMPCI)
            {                
                opcodeinfo.op = Instruction::BAD;
                opcodeinfo.opname=instructionInfo(Instruction::BAD).name;
            }

            if ((byte)Instruction::PUSH1 <= (byte)op &&
                    (byte)op <= (byte)Instruction::PUSH32)
            {
                vectorinfo.push_back (opcodeinfo);//�Ƚ���ǰ�Ĳ���

                size_t len=(byte)op - (byte)Instruction::PUSH1 + 1;//�м䶼�� �ֽ�value

                for( size_t j=i+1;j<(i+len+1);j++)
                {
                    OpcodeInfo _opcodeinfo(j,Instruction::STOP,"<Value_INSTRUCTION>"  );
                    vectorinfo.push_back(_opcodeinfo);
                }

                i +=len;
            }
            else
            {
                vectorinfo.push_back (opcodeinfo);//�Ƚ���ǰ�Ĳ���
            }
            

            //cout<<"i="<<i<<"	"<<instructionInfo(op).name<<"\n";

        }

        lasttime=utcTime();//utcTime��ʵ�� �Ѿ��ĳɺ�����
    }

	byte* code;
    bytes codespace;


    uint64_t    lasttime;  //������ʱ��    
    std::vector<OpcodeInfo> vectorinfo;

    void hint(size_t index,size_t codespacesize)
    {
        if(  codespace.size() > codespacesize  )
        {
            //˵������������ͨ���ף����ǲ����Լ������Ҫ����ƫ����
            index=index+(codespace.size()-codespacesize);
        }
        if( index < vectorinfo.size() )
        {
            vectorinfo.at(index).hint++;
            lasttime=utcTime();
        }
              
            
    }
   
};



class CoverTool 
{
private:   
    std::vector<std::thread> m_checks;
    std::map<Address, StatItem> m_statitems;    //��Լ��ַΪkey

    mutable SharedMutex  m_lock;	//��Ȼûɶ��Ҫ 
    bool m_aborting = false;
    std::string m_outputpath;//���·��

public:

	CoverTool() ;
	 ~CoverTool() ;

    void outputpath(std::string path)//��ô��������Ϊcovertool��vm���Ǿ�̬���Ա����ʼ��ʱdatadir��û�и���
    {
        m_outputpath=path;
    }
    bool has(Address address);
    void init(Address address,bytes cspace);
    void hint(Address address,size_t index,size_t codespacesize);


   
private:

    void output();
	

};



}
}

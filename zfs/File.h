/*
 * File.h
 *
 *  Created on: 2019年11月14日
 *      Author: 10080207
 *      文件存储的实现类
 */

#ifndef STORE_FILE_H_
#define STORE_FILE_H_

#include "../stdafx.h"
#include <iostream>
#include <fstream>


using namespace std;

namespace zfs
{

#define FILE_READ 0
#define FILE_WRITE 1

    class IFileInfo
    {


    };

    // 文件实现接口
    class IFile
    {
    public:
        virtual ~IFile();

        // 判断文件是否存在
        virtual bool Exists(const string& strDirName, const string& strFileName) = 0;

        virtual void InitFileInfo(IFileInfo* pFileInfo) {}

        // 设置打开文件位置，此方法给TrunkFile使用
        virtual void SetOpenReadPos(const unsigned int& nPos);
        // 打开文件操作, mod = 0，表示read,mod =1 表示 write
        // strDirName，文件目录
        // nas文件同文件目录+文件名组成,fastdfs由gropName+strFileName组成
        virtual bool Open(const string& strDirName, const string& strFileName, const int& mode) = 0;
        // 关闭文件操作
        virtual bool Close() = 0;
        // 读取文件操作
        virtual int Read(char* buffer, const unsigned int& size) = 0;
        // 写文件操作
        virtual bool Write(const char* buffer, const unsigned int& size) = 0;
        // 获取文件大小，注意，程序的通用性考虑，返回值 类型为long。超过了long的最大值的文件可能会出错。邮件系统中基本不会出现
        // 注意.nas文件 调用GetSize，会seek到文件头。
        virtual long GetSize() = 0;
        // 创建文件夹
        virtual int CreateDir(const string& strDirName) = 0;
        // nas文件同文件目录+文件名组成,fastdfs由gropName+strFileName组成
        virtual int Delete(const string& strDirName, const string& strFileName) = 0;
    };

    // NAS文件操作实现类
    class CNasFile : public IFile
    {
    public:
        CNasFile();
        virtual ~CNasFile();

        // 判断文件是否存在
        virtual bool Exists(const string& strDirName, const string& strFileName);

        // 打开文件操作, mod = 0，表示read,mod =1 表示 write
        bool OpenWriteEx(const string& strDirName, const string& strFileName);
        // 打开文件操作, mod = 0，表示read,mod =1 表示 write
        virtual bool Open(const string& strDirName, const string& strFileName, const int& mode);
        // 关闭文件操作
        virtual bool Close();
        // 读取文件操作
        virtual int Read(char* buffer, const unsigned int& size);
        // 写文件操作
        virtual bool Write(const char* buffer, const unsigned int& size);
        // 获取文件大小
        virtual long GetSize();
        // 创建文件夹
        virtual int CreateDir(const string& strDirName);

        // 删除文件，strDirName，文件目录,strFileName文件名
        // nas文件同文件目录+文件名组成,fastdfs由gropName+strFileName组成
        virtual int Delete(const string& strDirName, const string& strFileName);

        // 获取当前指针位置
        std::streampos GetPos();
        // 设置当前指针位置
        void SetPos(std::streampos pos);

        // 刷文件操作
        void Flush();
    protected:
        fstream* m_fStream;   // intput/output stream
    };

} /* namespace zfs */

#endif /* STORE_FILE_H_ */

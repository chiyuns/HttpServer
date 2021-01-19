/*
 * File.cpp
 *
 *  Created on: 2019年11月14日
 *      Author: 10080207
 */

#include "File.h"
#include <sys/stat.h>

#include <string.h>
#include "../utility/Util.h"

#ifdef WIN32
#include <Windows.h>
#endif

namespace zfs
{

    /////////  IFile 开始   //////////
    IFile::~IFile()
    {
        // TODO Auto-generated destructor stub
    }

    // 设置打开文件位置，此方法给TrunkFile文件使用
    void IFile::SetOpenReadPos(const unsigned int& nPos)
    {

    }

    /////////  IFile 结束   //////////

    /////////  CNasFile 开始   //////////
    CNasFile::CNasFile()
    {
        m_fStream = NULL;
    }
    CNasFile::~CNasFile()
    {
        Close();
    }

    // 判断文件是否存在
    bool CNasFile::Exists(const string& _strDirName, const string& _strFileName)
    {
        string strDirName;
        string strFileName;
        zchs::CUtil::utf8ConvertLocale(_strDirName, strDirName);
        zchs::CUtil::utf8ConvertLocale(_strFileName, strFileName);

        int nDirName = strDirName.length();
        int nFileName = strFileName.length();
        const int nLen = nDirName + nFileName + 3;
        char* cDir = new char[nLen];
        memset(cDir, 0, nLen);
        memcpy(cDir, strDirName.c_str(), nDirName);
        int nPos = nDirName - 1;
        if (cDir[nPos] != '\\' && cDir[nPos] != '/')
        {
            cDir[nPos + 1] = '/';
            nPos = nPos + 1;
        }
        memcpy(cDir + nPos + 1, strFileName.c_str(), nFileName);
#ifdef WIN32
        // 判断文件夹是否存在
        if (_access(cDir, F_OK) == 0)
        {
            delete[] cDir;
            return true;
        }
#else
        // 判断文件夹是否存在
        if (access(cDir, F_OK) == 0)
        {
            delete[] cDir;
            return true;
        }
#endif
        delete[] cDir;
        return false;
    }

    // 打开文件操作, mod = 0，表示read,mod =1 表示 write
    bool CNasFile::OpenWriteEx(const string& _strDirName, const string& _strFileName)
    {
        // 不能重复打开
        if (m_fStream != NULL)
        {
            return false;
        }

        int openMode = ios::out | ios::binary | ios::ate | ios::app;

        string strDirName;
        string strFileName;
        zchs::CUtil::utf8ConvertLocale(_strDirName, strDirName);
        zchs::CUtil::utf8ConvertLocale(_strFileName, strFileName);

        CreateDir(strDirName);

        int nDirName = strDirName.length();
        int nFileName = strFileName.length();
        int nLen = nDirName + nFileName + 3;
        char* cDir = new char[nLen];
        memset(cDir, 0, nLen);
        memcpy(cDir, strDirName.c_str(), nDirName);
        int nPos = nDirName - 1;
        if (cDir[nPos] != '\\' && cDir[nPos] != '/')
        {
            cDir[nPos + 1] = '/';
            nPos = nPos + 1;
        }
        memcpy(cDir + nPos + 1, strFileName.c_str(), nFileName);

        // 隐藏文件
        m_fStream = new fstream(cDir, (ios_base::openmode) openMode);
        if (m_fStream->is_open())
        {
            delete[] cDir;
            return true;
        }

        LOGE("CNasFile::Open error %s\n", cDir);
        delete[] cDir;
        return false;
    }

    // 打开文件操作, mod = 0，表示read,mod =1 表示 write
    bool CNasFile::Open(const string& _strDirName, const string& _strFileName, const int& mode)
    {
        // 不能重复打开
        if (m_fStream != NULL)
        {
            return false;
        }

        int openMode = 0;
        if (mode == 0)
        {
            // read
            openMode = ios::in | ios::binary;
        }
        else
        {
            // write
            openMode = ios::out | ios::binary;
        }

        string strDirName;
        string strFileName;
        zchs::CUtil::utf8ConvertLocale(_strDirName, strDirName);
        zchs::CUtil::utf8ConvertLocale(_strFileName, strFileName);

        // 如果是写，则创建文件夹
        if (mode == FILE_WRITE)
        {
            CreateDir(strDirName);
        }

        int nDirName = strDirName.length();
        int nFileName = strFileName.length();
        int nLen = nDirName + nFileName + 3;
        char* cDir = new char[nLen];
        memset(cDir, 0, nLen);
        memcpy(cDir, strDirName.c_str(), nDirName);
        int nPos = nDirName - 1;
        if (cDir[nPos] != '\\' && cDir[nPos] != '/')
        {
            cDir[nPos + 1] = '/';
            nPos = nPos + 1;
        }
        memcpy(cDir + nPos + 1, strFileName.c_str(), nFileName);

        m_fStream = new fstream(cDir, (ios_base::openmode) openMode);
        if (m_fStream->is_open())
        {
            delete[] cDir;
            return true;
        }
        else
        {
            int   savedErrno = 0;
#ifdef WIN32
            savedErrno = GetLastError();
#else
            savedErrno = errno;
#endif
            LOGE("CNasFile::Open error %s, GetLastError =%d\n", cDir, savedErrno);
            delete[] cDir;
            delete m_fStream;
            m_fStream = NULL;
            return false;
        }
    }
    // 关闭文件操作
    bool CNasFile::Close()
    {
        if (m_fStream != NULL)
        {
            m_fStream->close();
            delete m_fStream;
            m_fStream = NULL;
        }
        return true;
    }

    // 刷文件操作
    void CNasFile::Flush()
    {
        if (m_fStream != NULL)
        {
            m_fStream->sync();
        }
    }
    // 读取文件操作,把文件内容写入buffer, size是读取的文件长度。返回值为实际读取的文件大小
    int CNasFile::Read(char* buffer, const unsigned int& size)
    {

        if (m_fStream == NULL)
        {
            return -1;
        }
        // m_fStream->seekg (0, ios::beg);
        m_fStream->read(buffer, size);
        streamsize count = m_fStream->gcount();
        return count;
    }


    // 写文件操作
    bool CNasFile::Write(const char* buffer, const unsigned int& size)
    {
        if (m_fStream == NULL)
        {
            return false;
        }

        // m_fStream->seekg(0, ios::beg);
        m_fStream->write(buffer, size);

        return true;
    }
    // 获取文件大小
    long CNasFile::GetSize()
    {
        if (m_fStream == NULL)
        {
            return 0;
        }
        std::streampos oldPos = m_fStream->tellg();

        m_fStream->seekg(0, ios::end);      // 设置文件指针到文件流的尾部
        std::streampos pos = m_fStream->tellg();          // 读取文件指针的位置

        m_fStream->seekg(oldPos, ios::beg);
        if (pos == -1)
        {
            pos = 0;
        }
        return (long)pos;
    }

    // 获取当前指针位置
    std::streampos CNasFile::GetPos()
    {
        if (m_fStream == NULL)
        {
            return 0;
        }
        return  m_fStream->tellg();
    }

    // 设置当前指针位置
    void CNasFile::SetPos(std::streampos pos)
    {
        if (m_fStream == NULL)
        {
            return;
        }
        m_fStream->seekg(pos, ios::beg);
    }

    // 创建文件夹
    int CNasFile::CreateDir(const string& _strDir)
    {
        string strDir = _strDir;
        int i = 0;
        int iRet = 0;
        int iLen = strDir.length();
        char* pszDir = new char[iLen + 2];
        memset(pszDir, 0, iLen + 2);
        memcpy(pszDir, strDir.c_str(), iLen);

        //在末尾加/
        if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/')
        {
            pszDir[iLen] = '/';
            pszDir[iLen + 1] = '\0';
        }

        // 创建目录
        for (i = 1; i <= iLen; i++)
        {
            if (pszDir[i] == '\\' || pszDir[i] == '/')
            {
                pszDir[i] = '\0';
                //如果不存在,创建
#ifdef WIN32
                iRet = _access(pszDir, 0);
#else
                iRet = access(pszDir, 0);
#endif
                if (iRet != 0)
                {
#ifdef WIN32
                    iRet = _mkdir(pszDir);
#else
                    iRet = mkdir(pszDir, S_IRWXU | S_IRWXG | S_IRWXO);
#endif
                    if (iRet != 0)
                    {
                        delete[] pszDir;
                        return -1;
                    }

                }
                //支持linux,将所有\换成/
                pszDir[i] = '/';
            }
        }

        delete[] pszDir;
        return 0;

    }

    // 删除文件，strDirName，文件目录,strFileName文件名
    // nas文件同文件目录+文件名组成,fastdfs由gropName+strFileName组成
    int CNasFile::Delete(const string& _strDirName, const string& _strFileName)
    {
        string strDirName;
        string strFileName;
        zchs::CUtil::utf8ConvertLocale(_strDirName, strDirName);
        zchs::CUtil::utf8ConvertLocale(_strFileName, strFileName);

        // @todo
        if (!strDirName.empty() && !strFileName.empty())
        {
            int nLength = strDirName.length() + strFileName.length() + 3;
            char* strPath = new char[nLength];
            sprintf(strPath, "%s/%s", strDirName.c_str(), strFileName.c_str());

            int nResult = remove(strPath);
            delete[] strPath;
            return nResult;
        }
        else
        {
            return 0;
        }

    }


    /////////  CNasFile 结束   //////////

} /* namespace MD */

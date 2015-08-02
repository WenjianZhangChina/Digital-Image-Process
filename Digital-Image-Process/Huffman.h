// Huffman.h
// ����������ͷ�ļ�
#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// �ڵ�ṹ��
typedef struct huffNode
{
	int parent,lchild,rchild;
	unsigned long count;		// �ַ����ֵĴ���
	unsigned char c;			// ���ڵ��ַ�
	char bits[256];				// ���봮
}HuffNode;

#define N_Node 512				// 256���ַ�����Ҫ511���ڵ���ܹ��ɻ�������

class HuffCode
{

public:
	HuffCode():m_fpInfile(NULL), m_fpOutFile(NULL){}
	~HuffCode(){ xInit_Tree(); }

	// ���������ļ�ָ��
	void SetInputFile(FILE *fpInfile)
	{
		m_fpInfile = fpInfile;
	}

	// ��������ļ�ָ��
	void SetOutputFile(FILE *fpOutFile)
	{
		m_fpOutFile = fpOutFile;
	}

	// get����
	double GetRawFileEntropy();
	double GetAverCodeLen();
	double GetRate(){return double(m_nCompressFile)/double(m_nRawFile);}
	unsigned int GetRawSize() {return m_nRawFile;}
	unsigned int GetNUsedChar() {return m_nUsedChar;}
	unsigned int GetMaxCodeLen() {return m_nMaxCodeLen;}

	void BuildTree();				// ������������
	unsigned int EncodeFile();		// ����
	unsigned int DecodeFile();		// ����
		
public:
	HuffNode m_HTree[N_Node];		// �������ڵ�����

private:
	FILE *m_fpInfile;				// �����ļ�ָ��
	FILE *m_fpOutFile;				// ����ļ�ָ��
	unsigned int m_nRawFile;			// δѹ���ļ��ֽ���
	unsigned int m_nCompressFile;   // ѹ���ļ��ֽ���
	unsigned int m_nUsedChar;		// ʹ�õ����ַ�����
	unsigned int m_nMaxCodeLen;		// ����봮�ĳ���

private:
	void xInit_Tree();				// ��ʼ��ÿ���ڵ�   
};

#endif



// Huffman.cpp
// ����������ʵ���ļ�

#include "Huffman.h"
#include <math.h>


/**************************************************
unsigned int HuffCode::DecodeFile()
���ܣ�	���������ļ�ָ�������ļ�ָ�����л���������
������	��
����ֵ��	unsigned int������õ���ԭʼ�ļ��ֽ���
***************************************************/
unsigned int HuffCode::DecodeFile()
{
	xInit_Tree();
	if (NULL == m_fpInfile || NULL == m_fpOutFile)
	{
		return -1;
	}
	fseek(m_fpInfile, 0, 2);
	m_nCompressFile = ftell(m_fpInfile);
	fseek(m_fpInfile, 0, 0);
	// ��ȡͷ��Ϣ:
	// m_nRawFile��ԭʼ�ļ��ĳ���
	// m_nUsedChar:���õ��ַ���
	// m_nMaxCodeLen������볤
	// m_HTree:��������
	fread(&m_nRawFile, sizeof(unsigned int),1,m_fpInfile); 
	fread(&m_nUsedChar, sizeof(unsigned int),1,m_fpInfile);
	fread(&m_nMaxCodeLen, sizeof(unsigned int),1,m_fpInfile);
		
	unsigned int i;
	for(i=0; i<=m_nUsedChar; i++)
	{
		fread(&m_HTree[i].c, sizeof(unsigned char), 1, m_fpInfile);
		fread(&m_HTree[i].bits, m_nMaxCodeLen, 1, m_fpInfile);
	}

	// ��ʼ��ѹ�ļ�
	unsigned int nOutChar = 0;
	unsigned char c;		// ��ȡ���ַ�
	char ac_digital[256];	// �������ַ�תΪ�������ַ���
	char buffer[256];		// �ܵĶ������ַ���
	buffer[0] = '\0';
	while(true)
	{
		// ��ȡm_nMaxCodeLen��bit��buffer�� 
		while(strlen(buffer) < m_nMaxCodeLen) 
		{
			fread(&c,1,1,m_fpInfile); 
			int k=c; 
			_itoa_s(k, ac_digital, 2); 
			k=strlen(ac_digital); 
			int l;
			for(l=8;l>k;l--) //�ڵ��ֽ��ڶ���Ӧλ�ò�0
			{
				strcat(buffer,"0"); 
			}
			strcat(buffer,ac_digital); 
		}
		// buffer������е�ֵ�Ƚ�
		unsigned int i;
		for(i=0; i<=m_nUsedChar; i++) 
		{
			if( memcmp(m_HTree[i].bits, buffer, strlen(m_HTree[i].bits)) == 0)
			{
				break; 
			}
		}
		strcpy(buffer, buffer+strlen(m_HTree[i].bits)); 
		c = m_HTree[i].c; 
		fwrite(&c, 1, 1, m_fpOutFile); 
		m_HTree[i].count++;
		//ͳ�ƽ�ѹ�����ļ��ĳ��ȣ��ж��Ƿ��ѹ���
		nOutChar++;   
		if(nOutChar == m_nRawFile) 
			break;  
	}
	return nOutChar;
}

/**************************************************
unsigned int HuffCode::EncodeFile()
���ܣ�	���������ļ�ָ�������ļ�ָ�����л���������
������	��
����ֵ��	unsigned int��ѹ������ļ����ֽ���
***************************************************/
unsigned int HuffCode::EncodeFile()
{
	if (NULL == m_fpInfile || NULL == m_fpOutFile)
	{
		return -1;
	}

	// дͷ��Ϣ��
	// ԭ�ļ��ܳ��ȣ�4�ֽ�
	// �õ����ַ�������4�ֽ�
	// ����볤��4�ֽ�
	// ���������
	fseek(m_fpOutFile,0,0);
	fwrite(&m_nRawFile, sizeof(unsigned int), 1, m_fpOutFile);         
	fwrite(&m_nUsedChar, sizeof(unsigned int), 1, m_fpOutFile);
	fwrite(&m_nMaxCodeLen, sizeof(unsigned int), 1, m_fpOutFile);
	unsigned int i;
	for(i=0; i<=m_nUsedChar; i++)
	{
		fwrite(&m_HTree[i].c, sizeof(unsigned char), 1, m_fpOutFile);
		fwrite(&m_HTree[i].bits, m_nMaxCodeLen, 1, m_fpOutFile);
	}

	// ѹ�����ļ�
	char buf[N_Node];	// ��ű�����
	int j = 0;			// ��ȡλ���α�
	int c;				// ��ȡ���ַ�
	int k;
	unsigned int n_wr = 12+(1+m_nMaxCodeLen)*(m_nUsedChar+1); // д����ֽ���

	buf[0] = '\0';
	fseek(m_fpInfile, 0, SEEK_SET);
	while(!feof(m_fpInfile))
	{
		c = fgetc(m_fpInfile);
		j++;
		for(i = 0; i <= m_nUsedChar; i++)
		{
			if(m_HTree[i].c == c)
				break;
		}
		strcat(buf, m_HTree[i].bits);


		// �ۼƱ���λ������һ���ֽڣ������ļ�ִ��д��
		k = strlen(buf);
		c = 0;
		while(k>=8)
		{
			for(i=0;i<8;i++)
			{
				if(buf[i]=='1') 
					c=(c<<1)|1;
				else
					c=c<<1;
			}
			fwrite(&c,1,1,m_fpOutFile);
			n_wr++;
			strcpy(buf, buf+8);
			k = strlen(buf);
		}
		if(j == m_nRawFile)
			break;
	}
	if( k>0 )            //���ܻ���ʣ���ַ�
	{
		strcat(buf,"00000000");
		for(i=0; i<8; i++)
		{
			if(buf[i] == '1') 
				c = (c<<1)|1;
			else
				c = c<<1;
		}
		fwrite(&c, 1, 1, m_fpOutFile);
		n_wr++;
	}
	m_nCompressFile = n_wr;
	return n_wr;
}

/**************************************************
void HuffCode::xInit_Tree()
���ܣ�	��ʼ��ÿ�����������ڵ�
������	��
����ֵ��	��
***************************************************/
void HuffCode::xInit_Tree()
{
	int i;
	// -1��ʾû�и�ĸ����Ů
	for(i=0;i<N_Node;i++)
	{
		// ���ֵĴ���Ϊ��
		m_HTree[i].count=0;
		// ���ڵ��ʾ���ַ�
		m_HTree[i].c=(unsigned char)i;
		m_HTree[i].lchild=-1;
		m_HTree[i].parent=-1;
		m_HTree[i].rchild=-1;
	}
}

/**************************************************
double HuffCode::GetInFileEntropy()
���ܣ�	����δѹ���ļ�����ֵ
������	��
����ֵ��	double��δѹ���ļ�����ֵ
***************************************************/
double HuffCode::GetRawFileEntropy()
{
	if (NULL==m_fpInfile)
		return -1.0;
	unsigned int i;
	double entropy=0.0;
	
	for (i=0; i<=m_nUsedChar; i++)
	{
		
		if (m_HTree[i].count != 0)
		{
			double rate = 1.0*m_HTree[i].count/(double)m_nRawFile;
			entropy += rate*log(double(rate))/log(2.0);
		}
	}
	
	return -entropy;
}

/**************************************************
double HuffCode::GetAverCodeLen()
���ܣ�	��������ƽ���볤
������	��
����ֵ��	double�������ƽ���볤
***************************************************/
double HuffCode::GetAverCodeLen()
{
	if (NULL==m_fpInfile)
		return -1.0;
	unsigned int i;
	double len=0.0;
	int s=0;
	for (i=0; i<=m_nUsedChar; i++)
	{
		if (m_HTree[i].count != 0)
		{
			len+=strlen(m_HTree[i].bits)*m_HTree[i].count;
			s+=m_HTree[i].count;
		}
	}
	return len*1.0/s;;
}

/**************************************************
void HuffCode::BuildTree()
���ܣ�	����m_fpInfileָ�����ļ�����ȡ�ļ����ݣ�������������
������	��
����ֵ��	��
***************************************************/
void HuffCode::BuildTree()
{
	int i,j,k;

	// 1.��ʼ����
	xInit_Tree();

	// 2.ͳ�������ļ��и����ַ����ִ���
	m_nRawFile = 0;	
	unsigned char c; 
	while(!feof(m_fpInfile))
	{
		fread(&c,1,1,m_fpInfile);
		m_HTree[c].count++;
		m_nRawFile++;
	}
	m_nRawFile--;
	m_HTree[c].count--;

	// 3.countΪ0�Ĳ�Ҫ����count�Ӵ�С����
	HuffNode temp;
	for(i=0;i<255;i++)
	{
		for(j=i+1;j<256;j++)
		{
			if(m_HTree[i].count<m_HTree[j].count)
			{
				temp=m_HTree[i];
				m_HTree[i]=m_HTree[j];
				m_HTree[j]=temp;
			}
		}
	}

	// 4.ͳ�������ַ���
	for(i=0;i<256;i++)
		if(m_HTree[i].count==0)
			break;
	m_nUsedChar = i-1;   

	// 5.������������
	unsigned int min_count;		// count����Сֵ
	int m=2*i-1;				// ���������� m ���ڵ�
	int min_index;				// ��Сcountֵ�ڵ�����
	for(i=m_nUsedChar+1; i<m; i++)
	{
		min_count=UINT_MAX;
		for(j=0;j<i;j++)
		{
			// ��û�и��ڵ�Ľڵ���Ѱ����Сcountֵ�����и��ڵ�������
			if(m_HTree[j].parent!=-1)  
				continue;
			if(min_count > m_HTree[j].count)
			{
				min_index=j;
				min_count=m_HTree[min_index].count;
			}
		}
		// pt��i������
		m_HTree[i].count=min_count; 
		m_HTree[min_index].parent=i;
		m_HTree[i].lchild=min_index;

		min_count=UINT_MAX;
		for(j=0;j<i;j++)
		{
			// ��û�и��ڵ�Ľڵ���Ѱ����Сcountֵ�����и��ڵ�������
			if(m_HTree[j].parent!=-1) 
				continue;
			if(min_count>m_HTree[j].count)
			{
				min_index=j;
				min_count=m_HTree[min_index].count;
			}
		}
		// pt��i���Һ���
		m_HTree[i].count += min_count;
		m_HTree[min_index].parent=i;
		m_HTree[i].rchild=min_index;
	}

	// 6.Ϊÿ����Ȩֵ���ַ�����
	for(i=0; i<=m_nUsedChar; i++)
	{
		k=i;
		m_HTree[i].bits[0]=0;
		// һֱ����׷�ݣ�������ڵ�Ϊֹ����¼�����ڵ�ı�����ţ��γɱ����ַ������硰00110��
		while(m_HTree[k].parent != -1) 
		{
			j=k;
			k=m_HTree[k].parent;
			// ��ǰ�ڵ��Ǹ��ڵ����ڵ�
			// ����Ϊ0
			if(m_HTree[k].lchild==j)  
			{
				j=strlen(m_HTree[i].bits);  
				memmove(m_HTree[i].bits+1,m_HTree[i].bits,j+1);
				m_HTree[i].bits[0]='0';    
			}
			// ��ǰ�ڵ��Ǹ��ڵ���ҽڵ�
			// ����Ϊ1
			else						
			{
				j=strlen(m_HTree[i].bits); 
				memmove(m_HTree[i].bits+1, m_HTree[i].bits, j+1);
				m_HTree[i].bits[0]='1'; 
			}
		}
	}

	// 7.��¼��ı��볤��
    m_nMaxCodeLen=0;           
	for(i=0;i<=m_nUsedChar;i++)
	{
		if(m_nMaxCodeLen<strlen(m_HTree[i].bits))
			m_nMaxCodeLen=strlen(m_HTree[i].bits);
	}

}



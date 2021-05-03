#pragma once
#include <stdio.h>
#include<vector>

//�t�@�C������N���X
class FileFunction
{
public:
	template<class T>
	//�t�@�C���̓ǂݍ���
	static void Load(T& data, const char* fileName, const char* readMode)
	{
		FILE* fp;
		//�t�@�C�����J������
		if (fopen_s(&fp, fileName, readMode) != 0)return;
		//�t�@�C���̃T�C�Y��ǂݎ��
		long size = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		//�t�@�C������f�[�^��ǂݎ��
		fread(&data, size, 1, fp);
		//�t�@�C�������
		fclose(fp);
	}
	template<class T>
	//�t�@�C���̓ǂݍ���
	static void LoadArray(T* data, const char* fileName, const char* readMode)
	{
		FILE* fp;
		//�t�@�C�����J������
		if (fopen_s(&fp, fileName, readMode) != 0)return;
		//�t�@�C���̃T�C�Y��ǂݎ��
		long size = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int num = size / sizeof(T);
		//�t�@�C������f�[�^��ǂݎ��
		fread(&data[0], sizeof(T), num, fp);
		//�t�@�C�������
		fclose(fp);
	}
	template<class T>
	//�t�@�C���̓ǂݍ���
	static void LoadArray(std::vector<T>& data, const char* fileName, const char* readMode)
	{
		FILE* fp;
		//�t�@�C�����J������
		if (fopen_s(&fp, fileName, readMode) != 0)return;
		//�t�@�C���̃T�C�Y��ǂݎ��
		long size = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int num = size / sizeof(T);
		data.resize(num);
		//�t�@�C������f�[�^��ǂݎ��
		fread(&data[0], sizeof(T), num, fp);
		//�t�@�C�������
		fclose(fp);
	}

	template<class T>
	//�t�@�C���̏�������
	static void Save(T& data, const char* fileName, const char* writeMode)
	{
		FILE* fp;
		//�t�@�C�����J������
		fopen_s(&fp, fileName, writeMode);
		//�t�@�C���Ƀf�[�^����������
		fwrite(&data, sizeof(T), 1, fp);
		//�t�@�C�������
		fclose(fp);

	}
	template<class T>
	//�t�@�C���̏�������
	static void SaveArray(T* data, int num, const char* fileName, const char* writeMode)
	{
		FILE* fp;
		//�t�@�C�����J������
		fopen_s(&fp, fileName, writeMode);
		//�t�@�C���Ƀf�[�^����������
		fwrite(&data[0], sizeof(T), num, fp);
		//�t�@�C�������
		fclose(fp);

	}

};
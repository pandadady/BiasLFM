#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <stdlib.h>

using namespace std;

float averageScore;//平均分
const int userNum = 943;//用户数
const int itemNum = 1682;//项目数
const int factorNum = 10;//特征数
float learnRate = 0.005;//学习速率
float regularization = 0.05;//正则化系数

float bi[itemNum];//项目偏置项
float bu[userNum];//用户偏置项

float qi[itemNum][factorNum];//项目因子矩阵
float pu[userNum][factorNum];//用户因子矩阵


//计算待分解矩阵中所有评分的平均值
float Average(char* Filename)
{
	ifstream fin(Filename, ios::in);
	char line[100] = { 0 };
	string x = "";
	string y = "";
	string z = "";
	string v = "";

	float result = 0.0;
	int cnt = 0;

	while (fin.getline(line, sizeof(line)))
	{
		cnt++;
		stringstream word(line);
		word >> x;
		word >> y;
		word >> z;
		word >> v;

		result += atof(z.c_str());
	}

	fin.clear();
	fin.close();

	return result / cnt;
}

//计算两个向量內积
float InnerProduct(float *v1, float *v2, int size)
{
	float result = 0.0;
	for (int i = 0;i<size;i++)
		result += v1[i] * v2[i];
	return result;
}

//计算预测评分
float PredictScore(float av, float bu, float bi, float *pu, float *qi)
{
	float pScore;
	pScore = av + bu + bi + InnerProduct(pu, qi, factorNum);
	if (pScore<1)
		pScore = 1;
	else if (pScore>5)
		pScore = 5;
	return pScore;
}

//计算RMSE
float Validate(char *testDataFile, float av, float *bu, float *bi, float pu[][factorNum], float qi[][factorNum])
{
	int cnt = 0;
	float rmse = 0.0;
	ifstream f(testDataFile, ios::in);
	char line3[1024];

	string v = "";
	string x = "";
	string y = "";
	string z = "";

	while (f.getline(line3, sizeof(line3)))
	{
		stringstream word(line3);
		word >> v;
		word >> x;
		word >> y;
		word >> z;

		cnt++;
		int uid = atoi(v.c_str()) - 1;
		int iid = atoi(x.c_str()) - 1;
		float pScore = PredictScore(av, bu[uid], bi[iid], pu[uid], qi[iid]);
		float tScore = atoi(y.c_str());

		rmse += (tScore - pScore)*(tScore - pScore);
	}

	f.clear();
	f.close();

	return sqrt(rmse / cnt);
}

//训练模型
void Train(char *testDataFile, char *trainDataFile)
{

	for (int i = 0;i<itemNum;i++)
		bi[i] = 0.0;

	for (int j = 0;j<userNum;j++)
		bu[j] = 0.0;

	float s = sqrt(factorNum);
	srand((unsigned)time(NULL));

	for (int i = 0;i<itemNum;i++)
		for (int j = 0;j<factorNum;j++)
			qi[i][j] = (rand() % 1) / s;

	for (int i = 0;i<userNum;i++)
		for (int j = 0;j<factorNum;j++)
			pu[i][j] = (rand() % 1) / s;

	cout << "initialization end" << endl << "start training..." << endl;

	float preRmse = 1000000.0;

	for (int step = 1;step <= 100;step++)
	{
		ifstream f(trainDataFile, ios::in);
		char line2[1024];
		string p = "";;
		string q = "";
		string u = "";
		string v = "";

		while (f.getline(line2, sizeof(line2)))
		{
			stringstream word(line2);
			word >> p;
			word >> q;
			word >> u;
			word >> v;

			int uid = atoi(p.c_str()) - 1;
			int iid = atoi(q.c_str()) - 1;
			float score = atof(u.c_str());
			float prediction = PredictScore(averageScore, bu[uid], bi[iid], pu[uid], qi[iid]);

			float eui = score - prediction;

			bu[uid] += learnRate*(eui - regularization*bu[uid]);
			bi[iid] += learnRate*(eui - regularization*bi[iid]);

			for (int k = 0;k<factorNum;k++)
			{
				float temp = pu[uid][k];
				pu[uid][k] += learnRate*(eui*qi[iid][k] - regularization*pu[uid][k]);
				qi[iid][k] += learnRate*(eui*temp - regularization*qi[iid][k]);
			}
		}

		f.clear();
		f.close();

		float curRmse = Validate(testDataFile, averageScore, bu, bi, pu, qi);
		cout << "test_RMSE in step:" << step << " = " << curRmse << endl;
		if (curRmse >= preRmse)
			break;
		else
			preRmse = curRmse;
	}

	cout << "model generation over" << endl;

}


//预测分数
void Predict(char *testDataFile, char *resultSaveFile)
{
	ifstream f1(testDataFile, ios::in);

	ofstream f2(resultSaveFile, ios::out);

	char line4[1024];
	string v = "";
	string x = "";
	string y = "";
	string z = "";

	while (f1.getline(line4, sizeof(line4)))
	{
		stringstream word(line4);
		word >> v;
		word >> x;
		word >> y;
		word >> z;

		int uid = atoi(v.c_str()) - 1;
		int iid = atoi(x.c_str()) - 1;

		float pScore = PredictScore(averageScore, bu[uid], bi[iid], pu[uid], qi[iid]);

		f2 << pScore << endl;
	}

	f1.close();
	f2.close();
	cout << "predict over" << endl;

}

int main()
{
	char *trainDataFile = "u1.base";//训练集
	char *testDataFile = "u1.test";//测试集
	char *resultSaveFile = "prediction.txt";//预测结果

	averageScore = Average("u.data");//平均分

	cout << "average score = " << averageScore << endl;

	Train(testDataFile, trainDataFile);//训练
	Predict(testDataFile, resultSaveFile);//预测

	system("pause");
	return 0;
}
#include "pch.h"
#include "mariaDB.h"

mariaDB::mariaDB()
{
	AttachDB();
}

mariaDB::~mariaDB()
{
	DetachDB();
}

/*
  desc: DB�� �����Ѵ�.
  param: ������ DB�� name
*/
void mariaDB::AttachDB()
{
	mysql_init(&Connect); // Connect�� pre-compiled header�� ���������� ���ǵǾ� ����.

	if (mysql_real_connect(&Connect, CONNECT_IP, DB_USER, DB_PASSWORD, DB_NAME, DB_PORT, NULL, 0))
	{
#ifdef _DEBUG 
		printf("DB ���Ἲ��!!!\n");
#endif
	}
	else
	{
#ifdef _DEBUG 
		printf("DB �������...\n");
#endif
	}

	mysql_query(&Connect, "SET Names euckr"); // DB ���� ���ڵ��� euckr�� ����
}

/*
  desc: DB�� ������ �����Ѵ�.
  param: ������ DB�� name
*/
void mariaDB::DetachDB()
{
	mysql_close(&Connect);
#ifdef _DEBUG 
	printf("DB ��������...\n");
#endif
}

BOOL mariaDB::get_img_path(CString UID, CString& img_path)
{
	std::string query;
	query = std::string("SELECT img_path FROM person WHERE uid = '")
		+ std::string(CT2CA(UID)) + std::string("';");

	mysql_query(&Connect, query.c_str());
	sql_query_result = mysql_store_result(&Connect);

	if (mysql_num_rows(sql_query_result) == 0)
	{
		return FALSE;
	}
	else
	{
		sql_row = mysql_fetch_row(sql_query_result);
		if (sql_row != nullptr)
		{
			img_path = sql_row[0];
		}
	}
	mysql_free_result(sql_query_result);
	return TRUE;
}


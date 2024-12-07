#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include <variant>
#include <map>
#include <string>

class GlobalVariables{
private:

	//項目
	struct Item {
		//項目の値
		std::variant<int32_t, float, Vector3, Vector2> value;
	};
	//グループ
	struct Group {
		std::map<std::string, Item> items;
	};
	//全データ
	std::map<std::string, Group> datas_;

	//グローバル変数の保存先のファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";


public:
	/// <summary>
	/// グループの作成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);
	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update();
	//値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	//値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	//値のセット(Vec3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);
	//値のセット(Vec2)
	void SetValue(const std::string& groupName, const std::string& key, const Vector2& value);
	/// <summary>
	/// ファイルの書き出し
	/// </summary>
	/// <param name="groupName">グループ</param>
	void SaveFile(const std::string& groupName);
	/// <summary>
	/// ディレクトリの全ファイル読み込み
	/// </summary>
	void LoadFiles();
	/// <summary>
	/// ファイルから読み込む
	/// </summary>
	/// <param name="groupName">グループ</param>
	void LoadFile(const std::string& groupName);
	//項目の追加(int)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	//項目の追加(float)
	void AddItem(const std::string& groupName, const std::string& key, float value);
	//項目の追加(Vec3)
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);
	//項目の追加(Vec2)
	void AddItem(const std::string& groupName, const std::string& key, const Vector2& value);

	//値の取得
	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	float GetFloatValue(const std::string& groupName, const std::string& key) const;
	Vector3 GetVec3Value(const std::string& groupName, const std::string& key) const;
	Vector2 GetVec2Value(const std::string& groupName, const std::string& key) const;

public:

	static GlobalVariables* GetInstance();

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	GlobalVariables& operator=(const GlobalVariables&) = delete;

};


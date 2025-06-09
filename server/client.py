# client.py
import requests

def send_log(user_id: int, message: str):
    url = "http://127.0.0.1:8000/logs"
    data = {
        "message": message,
        "user_id": user_id
    }
    response = requests.post(url, json=data)
    try:
        response.raise_for_status()
        print("✅ ログ送信成功:")
        print(response.json())
    except requests.exceptions.RequestException as e:
        print("❌ ログ送信失敗:", response.status_code)
        print(response.text)

def send_user(username: str, password: str):
    url = "http://127.0.0.1:8000/users"
    data = {
        "username": username,
        "password": password
    }
    response = requests.post(url, json=data)
    try:
        response.raise_for_status()  # ステータスコードがエラーなら例外
        print("✅ ユーザー登録成功:")
        print(response.json())
        return response.json()  # ユーザーIDなどを取得したい場合
    except requests.exceptions.HTTPError as e:
        print(f"❌ HTTPエラー: {e.response.status_code} - {e.response.text}")
    except requests.exceptions.RequestException as e:
        print("❌ エラー:", response.status_code)
        print(response.text)
        return None
    except Exception as e:
        print(f"❌ その他のエラー: {e}")

if __name__ == "__main__":
    # テスト送信
    user = send_user("goro", "pass123")
    if user:
        send_log(user_id=user["user_id"], message="first log")
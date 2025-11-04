![Videotogif (1)](https://github.com/user-attachments/assets/89569549-72fb-475f-8449-eba26335fed8)

<br/>
<h1 align="center">🔑 <a href="https://newhotkey.vercel.app">KeylogVisualization</a></h1><br/>
<p align="center">キーボードの押下ログの可視化サイトです。</p><br/>

## URL
[webページ](https://newhotkey.vercel.app)
- こちらよりアクセスが可能です。
- アクセス時には私のキーログを基に、チャートが表示されます。

- 新しいパネルボタンからはオリジナルにカスタマイズしたチャートを表示することができます。
- サンプルボタンはワンクリックでチャートが表示されます。

[Figma 設計図(DFD/システム構成図/etc...)](https://www.figma.com/design/1mnWRrs2KoO3GsV7ktOTae/keylogvisualization?node-id=0-1&p=f&t=1hmgQ5UvnzGZv1Qs-0)

[API 設計図](/api.md)

<br/>

## Concept
普段なにげなく使っているキーボードですが、<br/>
どのアプリでどのくらい使っているのかわかりやすく体感するために制作しました。<br/>
付属のCLIキーロガーを使用することでキーの押下ログを保存できます。<br/>
保存した押下ログを可視化するwebサイトです。<br/>
<br/>

## DB設計
| DB識別子 | カラム名 | 例 | 説明 |
|-|-|-|-|
| PK | id | 1 | レコードを一意に識別するためのインクリメントされるid |
| | session_id | 1_20251101T0101_1234 | (累積セッション回数)\_(年月日T時分)\_(アプリ起動時に決定されるランダムな4桁整数) |
| | sequence_id | 1 | セッション内で1からインクリメントされるid |
| | key | A | 入力されたキー |
| | modifiers | shift | 入力された修飾子 |
| | window_title | C:\WINDOWS\system32\cmd.exe | アクティブウィンドウのタイトル |
| | process_name | WindowsTerminal | アクティブウィンドウのプロセス名 |
| FK | user_id | 1 | ユーザ毎に割り当てられたid |
 
## Technologies
<p align="left">
  <a href="https://www.typescriptlang.org/"><img src="https://cdn.worldvectorlogo.com/logos/typescript.svg" height="50px;" /></a>
  <a href="https://nextjs.org/"><img src="https://cdn.worldvectorlogo.com/logos/next-js.svg" height="50px;" /></a>
  <a href="https://ja.reactjs.org/"><img src="https://cdn.worldvectorlogo.com/logos/react-2.svg" height="50px;" /></a>
  <a href="https://vercel.com/"><img src="https://user-images.githubusercontent.com/65433193/118944114-3b393980-b98f-11eb-84a5-fc9a1db8ea6b.png" height="50px;" /></a
</p>

| カテゴリ | 技術 |
|-----------|----------------|
| フロントエンド | Next.js 15.0.3 / React 18.3.1/ TypeScript 5 |
| バックエンド | Python 3.13.9 |
| データベース | PostgreSQL 15.5 |
| 環境構築 | Docker |
| CI/CD | Vercel |
| インフラ | Vercel / Render / Neon |
| その他 | Linter: ESLinter / テスト: jest |

<br/>

## DataFlow
![DFDコピー](https://github.com/user-attachments/assets/cf47daa4-8571-4175-a9ba-766c3e713554)

## 技術的な挑戦
- フロントエンドから、バックエンド、インフラ構築、DB設計まで一貫しての開発
- フロントエンドにReact+Rechartsを用い、APIから取得したデータをもとに動的にグラフを描画
- バックエンドにFastAPIを使い、ログデータの登録/検索を行うAPIを実装
- DBスキーマをalembic(マイグレーションツール)で管理
- フロントエンドでのユーザー入力に応じた、FastAPIでの動的な集計系クエリ生成
- Docker composeを用いたフロントエンド/バックエンド/DBの一元管理
- Vercelでフロントエンド、RenderでFastAPI、NeonでDBをホスト
- xssやSQLインジェクションなどセキュリティを意識した設計、開発
<br/>
  
## 開発全体で悩んだ点
- TypeScriptでよく使われる関数式の使い方
- 状態(state)や副作用(Effect)、コンポーネント引数(Props)の概念
- apiサーバやDBサーバが再起動中の時のフロントの表示
- 使用メモリを削減しつつDockerでコンテナを立てるための環境設定
- SQLインジェクション対策と動的なクエリ生成の両立
- I/OスパイクによるDBレコードの消失と復元、原因の特定
<br/>

## 今後の改善点
https://github.com/ripidot/newhotkey/issues/14

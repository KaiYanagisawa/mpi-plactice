# 1. ファイル説明

## 1.1. 逐次関係
### 1.1.1. run.sh
- 概要
  - ./sw-sampleを実行
  - 実行時間の標準出力・log書き込み
  - 第1引数/output.sampleと一致判定の出力
- Usage:
  $ run.sh [データセット名, デフォルト値: p0]

### 1.1.2. sw-sample.c
- 概要
  - 配布された相同性検索の逐次処理サンプルコード
- Usage:
  $ gcc sw-sample.c -o sw-sample

## 1.2. 並列化関係
### 1.2.1. run-mpi.sh
- 概要
  - ./sw-mpi && ./sw-sort を実行
  - 合計実行時間の標準出力・log書き込み
  - 第1引数/output.sampleと一致判定の出力
- Usage:
  $ run-mpi [データセット名, デフォルト値: p0] [RankSize, デフォルト値: 2]

### 1.2.2. sw-mpi.c
- 概要
  - sw-sampleの並列化コード
- Usage:
  $ mpicc sw-mpi.c -o sw-mpi

### 1.2.3. sw-sort.c
- 概要
  - outputファイルの読み込み、query_idを昇順にソート
  - query_idが同じ場合database内の出力順にソート
- Usage:
  $ gcc sw-sort.c -o sw-sort

## 1.3. その他
### 1.3.1. data_analysis.c
- 概要
  - データセットの分布を標準出力
- Usage:
  $ gcc data_analysis.c -o data_analysis && ./data_analysis [データセット名/(query or database)]

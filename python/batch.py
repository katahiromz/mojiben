import subprocess

## 空のファイルを作成（初期化）
#for grade in range(1, 7):
#    with open(f'grade{grade}.txt', 'w', encoding='utf-8') as f:
#        pass  # 空ファイルを作るだけ

# 入力ファイルを読み込んで処理
with open('ex.txt', 'r', encoding='utf-8') as infile:
    for line in infile:
        line = line.strip()
        if not line:
            continue
        result = subprocess.run(['./kanjigrade', line], capture_output=True)
        err = result.returncode
        if 1 <= err <= 6:
            with open(f'grade{err}.txt', 'a', encoding='utf-8') as outfile:
                outfile.write(line + '\n')

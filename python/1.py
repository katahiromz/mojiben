import subprocess
import regex

kanji_strings = "一右雨円王音下火花貝学気九休玉金空月犬見五口校左三山子四糸字耳七車手十出女小上森人水正生青夕石赤千川先早草足村大男竹中虫町天田土二日入年白八百文木本名目立力林六"
kanjis = regex.findall(r'\p{Script=Han}', kanji_strings)

hash_str = {}
for k in kanjis:
    hash_str[k] = ""

#print(kanjis)

# 入力ファイルを読み込んで処理
with open('grade1.txt', 'r', encoding='utf-8') as infile:
    for line in infile:
        line = line.strip()
        if not line:
            continue
        found = regex.findall(r'\p{Script=Han}', line)
        targets = {}
        for item in found:
            if item in kanjis:
                targets[item] = line
        candiate = ''
        shortest = ''
        for target in targets:
            if shortest == '':
                shortest = target
            else:
                if len(hash_str[shortest]) > len(hash_str[target]):
                    shortest = target
        if shortest != '':
            if len(hash_str[shortest]) > 0:
                hash_str[shortest] += "／"
            hash_str[shortest] += line

for i in kanjis:
    print(i + " = " + hash_str[i])

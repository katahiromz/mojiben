import subprocess
import regex

kanji_strings = "引羽雲園遠何科夏家歌画回会海絵外角楽活間丸岩顔汽記帰弓牛魚京強教近兄形計元言原戸古午後語工公広交光考行高黄合谷国黒今才細作算止市矢姉思紙寺自時室社弱首秋週春書少場色食心新親図数西声星晴切雪船線前組走多太体台地池知茶昼長鳥朝直通弟店点電刀冬当東答頭同道読内南肉馬売買麦半番父風分聞米歩母方北毎妹万明鳴毛門夜野友用曜来里理話"
kanjis = regex.findall(r'\p{Script=Han}', kanji_strings)

hash_str = {}
for k in kanjis:
    hash_str[k] = ""

#print(kanjis)

# 入力ファイルを読み込んで処理
with open('grade2.txt', 'r', encoding='utf-8') as infile:
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

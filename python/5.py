import subprocess
import regex

kanji_strings = "圧囲移因永営衛易益液演応往桜可仮価河過快解格確額刊幹慣眼紀基寄規喜技義逆久旧救居許境均禁句型経潔件険検限現減故個護効厚耕航鉱構興講告混査再災妻採際在財罪殺雑酸賛士支史志枝師資飼示似識質舎謝授修述術準序招証象賞条状常情織職制性政勢精製税責績接設絶祖素総造像増則測属率損貸態団断築貯張停提程適統堂銅導得毒独任燃能破犯判版比肥非費備評貧布婦武復複仏粉編弁保墓報豊防貿暴脈務夢迷綿輸余容略留領歴"
kanjis = regex.findall(r'\p{Script=Han}', kanji_strings)

hash_str = {}
for k in kanjis:
    hash_str[k] = ""

#print(kanjis)

# 入力ファイルを読み込んで処理
with open('grade5.txt', 'r', encoding='utf-8') as infile:
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

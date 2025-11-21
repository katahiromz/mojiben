import subprocess
import regex

kanji_strings = "悪安暗医委意育員院飲運泳駅央横屋温化荷界開階寒感漢館岸起期客究急級宮球去橋業曲局銀区苦具君係軽血決研県庫湖向幸港号根祭皿仕死使始指歯詩次事持式実写者主守取酒受州拾終習集住重宿所暑助昭消商章勝乗植申身神真深進世整昔全相送想息速族他打対待代第題炭短談着注柱丁帳調追定庭笛鉄転都度投豆島湯登等動童農波配倍箱畑発反坂板皮悲美鼻筆氷表秒病品負部服福物平返勉放味命面問役薬由油有遊予羊洋葉陽様落流旅両緑礼列練路和"
kanjis = regex.findall(r'\p{Script=Han}', kanji_strings)

hash_str = {}
for k in kanjis:
    hash_str[k] = ""

#print(kanjis)

# 入力ファイルを読み込んで処理
with open('grade3.txt', 'r', encoding='utf-8') as infile:
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

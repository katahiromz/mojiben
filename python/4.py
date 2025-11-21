import subprocess
import regex

kanji_strings = "愛案以衣位茨印英栄媛塩岡億加果貨課芽賀改械害街各覚潟完官管関観願岐希季旗器機議求泣給挙漁共協鏡競極熊訓軍郡群径景芸欠結建健験固功好香候康佐差菜最埼材崎昨札刷察参産散残氏司試児治滋辞鹿失借種周祝順初松笑唱焼照城縄臣信井成省清静席積折節説浅戦選然争倉巣束側続卒孫帯隊達単置仲沖兆低底的典伝徒努灯働特徳栃奈梨熱念敗梅博阪飯飛必票標不夫付府阜富副兵別辺変便包法望牧末満未民無約勇要養浴利陸良料量輪類令冷例連老労録"
kanjis = regex.findall(r'\p{Script=Han}', kanji_strings)

hash_str = {}
for k in kanjis:
    hash_str[k] = ""

#print(kanjis)

# 入力ファイルを読み込んで処理
with open('grade4.txt', 'r', encoding='utf-8') as infile:
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

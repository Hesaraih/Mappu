■ffhook_Mappu.dll
焼肉にあるffhook.dllの改訂版
MappuをFF11のテキストコマンドで操作するためのライブラリ

■出来る事
・倍率の変更
  /m z 1.0
・終了
  /m e
・NPCLISTの表示/非表示
  /m n
・NPCLISTの更新
  /m u
・対象をセット
  /m s <t>
  /m s Name
  /m s 0AF
  /m sa all
  /m sa <t>
・対象をクリア
  /m c <t>
  /m c Name
  /m c 0AF
  /m ca all
  /m ca <t>
・対象のタイプを変更
  /m t0 <t>
  /m t0 01D
  /m t1 <t>
  /m t1 1DF
  /m t2 <t>
  /m t2 2FD
ffhook_Mappu.iniにコマンドの簡単な説明が記載してありますので参考にして下さい
デフォルトの設定では開始コマンドは/m
オプションコマンドはz,e,n,u
引数をとるオプションコマンドはs,sa,c,ca,t0,t1,t2


■使うには
ffhook_Mappu.dll、ffhook_Mappu.ini、madCHook.dllの3つをMappu.exeのあるフォルダにコピー

■更新履歴
1.0.3.0 20130327VUに伴いpattern_cmdを変更
1.0.2.0 FF11_GetCmdで範囲外の配列を参照していたのを回避
1.0.1.0 1.0.0.2をMappu用に改訂
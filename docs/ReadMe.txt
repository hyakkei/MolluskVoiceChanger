================================================================
  Mollusk Voice Changer  v1.1.0
  © 2026 Ito Hyakkei
================================================================

【概要】
  声を軟体生物（イカ・タコ）風にリアルタイム加工する、
  通話・配信向けボイスチェンジャーです。


【動作環境】
  OS      : Windows 10 / 11（64bit）/ MacOS
  必須    : 仮想オーディオデバイス
            ■Win
            VB-CABLE Virtual Audio Device（無料）
              https://vb-audio.com/Cable/
            ■Mac
            BlackHole (無料)
              https://existential.audio/blackhole/


【初回セットアップ】

  1. 仮想オーディオデバイス をインストールし、PC を再起動する。

  2. Mollusk Voice Changer を起動し、
     「AUDIO SETTINGS」から以下を設定する。

       入力デバイス  : お使いのマイク
       出力デバイス  : 仮想オーディオデバイスの入力
                      (例)CABLE Input (VB-Audio Virtual Cable)

  3. お使いの通話・配信アプリの設定で、
     マイク（入力）デバイスを仮想オーディオデバイスの出力に変更する
     (例)CABLE Output (VB-Audio Virtual Cable)

  [!Tips]
  Disccord等の通話アプリケーションを使用の際は、
  通話アプリケーション側のノイズキャンセルによって、
  加工された声がキャンセリングされることがあります。
  通話アプリケーション側のノイズキャンセル機能をOFFにすることをおすすめします。


【基本的な使い方】

  ■ プリセットを選ぶ
    画面上部の 4 つのボタン（Squid Girl / Boy、Octopus Girl / Boy）
    から好みのキャラクターを選択する。

  ■ Basic モード（デフォルト）
    画面上部の 2 本のレベルメーターで入出力の音量を確認できる。
    （INPUT: エフェクト前、OUTPUT: エフェクト後）

    7 つのノブで音声を調整する。

      INPUT SENSITIVITY  マイクの感度（ノイズゲート）
      PITCH              声の高さ
      BRIGHTNESS         声の明るさ（フォルマント）
      BUBBLE             揺らぎ（LFO フィルター）
      SLIMY              ぬめり感（フェイザー）
      LO-FI              こもり感（バンドパスフィルター）
      VOLUME             出力音量

  ■ Advanced モード
    「ADVANCED」ボタンで切り替え。
    各エフェクトを個別に細かく調整できる。

  ■ プリセットの保存・読み込み
    「EXPORT PRESET」/ 「IMPORT PRESET」で
    現在の設定を XML ファイルとして保存・読み込みできる。

  ■ 終了と次回起動
    終了時に設定が自動保存され、次回起動時に復元される。


【ライセンス】

  本ソフトウェアは GNU Affero General Public License v3.0（AGPLv3）のもとで
  公開されています。
  ソースコードの複製・改変・再配布は、同ライセンスの条件に従う限り自由に行えます。

  ライセンス全文: https://www.gnu.org/licenses/agpl-3.0.html
  Copyright (C) 2026 Ito Hyakkei


【使用しているライブラリ・フォント】

  JUCE Framework
    © Raw Material Software Limited
    https://juce.com/

  Bungee / RocknRoll One / Alfa Slab One / Rubik Glitch
    Google Fonts（SIL Open Font License 1.1）


【免責事項】

  本ソフトウェアは現状のまま提供されます。
  使用によって生じたいかなる損害についても、
  作者は一切の責任を負いません。


【連絡先】

  Mail : ito.7474@gmail.com
  X    : @ito_74


================================================================
----------------------------------------------------------------
  English
----------------------------------------------------------------
================================================================

  Mollusk Voice Changer  v1.1.0
  © 2026 Ito Hyakkei

================================================================

[Overview]
  A real-time voice changer for calls and streaming that
  transforms your voice to sound like a mollusk (squid/octopus).


[System Requirements]
  OS       : Windows 10 / 11 (64-bit) / macOS
  Required : Virtual audio device
               Windows : VB-CABLE Virtual Audio Device (free)
                           https://vb-audio.com/Cable/
               Mac     : BlackHole (free)
                           https://existential.audio/blackhole/


[Initial Setup]

  1. Install the virtual audio device and restart your PC.

  2. Launch Mollusk Voice Changer and open "AUDIO SETTINGS".
     Configure as follows:

       Input device  : Your microphone
       Output device : Virtual audio device input
                       (e.g.) CABLE Input (VB-Audio Virtual Cable)

  3. In your call or streaming application, change the
     microphone (input) device to the virtual audio device output.
     (e.g.) CABLE Output (VB-Audio Virtual Cable)

  [!Tips]
  Some communication apps (e.g. Discord) may use noise
  cancellation that filters out the processed voice.
  It is recommended to disable noise cancellation in the app.


[Basic Usage]

  ■ Select a Preset
    Choose from the four buttons at the top of the screen:
    Squid Girl / Squid Boy / Octopus Girl / Octopus Boy.

  ■ Basic Mode (default)
    Two level meters at the top of the screen show input and
    output levels in real time.
    (INPUT: before effects, OUTPUT: after effects)

    Adjust your voice with 7 knobs:

      INPUT SENSITIVITY  Microphone sensitivity (noise gate)
      PITCH              Voice pitch
      BRIGHTNESS         Voice brightness (formant)
      BUBBLE             Wobble (LFO filter)
      SLIMY              Sliminess (phaser)
      LO-FI              Muffled tone (band-pass filter)
      VOLUME             Output volume

  ■ Advanced Mode
    Click "ADVANCED" to access per-effect fine-tuning.

  ■ Preset Export / Import
    Save and load your settings as XML files via
    "EXPORT PRESET" / "IMPORT PRESET".

  ■ Session Restore
    Settings are saved automatically on exit and
    restored on next launch.


[License]

  This software is released under the GNU Affero General Public License v3.0 (AGPLv3).
  You are free to copy, modify, and redistribute the source code under the terms
  of this license.

  Full license text: https://www.gnu.org/licenses/agpl-3.0.html
  Copyright (C) 2026 Ito Hyakkei
  

[Credits]

  JUCE Framework
    © Raw Material Software Limited
    https://juce.com/

  Bungee / RocknRoll One / Alfa Slab One / Rubik Glitch
    Google Fonts (SIL Open Font License 1.1)


[Disclaimer]

  This software is provided "as is", without warranty of any kind.
  The author shall not be liable for any damages arising from
  the use of this software.


[Contact]

  Mail : ito.7474@gmail.com
  X    : @ito_74


================================================================

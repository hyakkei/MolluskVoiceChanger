================================================================
  Mollusk Voice Changer  VST3 Plugin  v1.1.0
  © 2026 Ito Hyakkei
================================================================

【概要】
  声を軟体生物（イカ・タコ）風にリアルタイム加工する
  ボイスチェンジャーの VST3 プラグイン版です。
  DAW や VST3 対応ホストアプリケーションで使用します。


【動作環境】
  OS   : Windows 10 / 11（64bit）
         macOS 12 Monterey 以降（Intel / Apple Silicon）
  Host : VST3 対応の DAW またはホストアプリケーション
         （Ableton Live / Cubase / Reaper / VoiceMeeter など）
  I/O  : モノラル入力 → ステレオ出力


【インストール】

  ■ Windows

    1. 「Mollusk Voice Changer.vst3」フォルダを
       以下のフォルダにコピーする。

         C:\Program Files\Common Files\VST3\

    2. DAW でプラグインを再スキャンする。

    3. モノラル入力トラック（マイクチャンネルなど）に
       プラグインをインサートする。


  ■ macOS

    1. 「Mollusk Voice Changer.vst3」を
       以下のフォルダにコピーする。

         ~/Library/Audio/Plug-Ins/VST3/
         （全ユーザー共通の場合は /Library/Audio/Plug-Ins/VST3/）

    2. DAW でプラグインを再スキャンする。

    3. モノラル入力トラック（マイクチャンネルなど）に
       プラグインをインサートする。

    [!] Gatekeeper について
        本プラグインは署名されていないため、初回起動時に
        macOS がブロックする場合があります。
        その場合は以下の手順で許可してください。

          「システム設定」→「プライバシーとセキュリティ」
          →「このまま開く」または「許可」をクリック


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
    各エフェクトを個別に細かく調整でき、
    DAW からパラメーターをオートメーションで操作できる。

  ■ プリセットの保存・読み込み
    「EXPORT PRESET」/ 「IMPORT PRESET」で
    現在の設定を XML ファイルとして保存・読み込みできる。

  ■ DAW によるセッション保存
    プロジェクトを保存すると設定が自動的に保存され、
    次回プロジェクトを開いたときに復元される。


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

  Mollusk Voice Changer  VST3 Plugin  v1.1.0
  © 2026 Ito Hyakkei

================================================================

[Overview]
  A VST3 plugin version of the Mollusk Voice Changer that
  transforms your voice to sound like a squid or octopus.
  Use it in any VST3-compatible DAW or host application.


[System Requirements]
  OS   : Windows 10 / 11 (64-bit)
         macOS 12 Monterey or later (Intel / Apple Silicon)
  Host : Any VST3-compatible DAW or host application
         (Ableton Live / Cubase / Reaper / VoiceMeeter, etc.)
  I/O  : Mono input → Stereo output


[Installation]

  ■ Windows

    1. Copy the "Mollusk Voice Changer.vst3" folder to:

         C:\Program Files\Common Files\VST3\

    2. Rescan plugins in your DAW.

    3. Insert the plugin on a mono input track
       (e.g. a microphone channel).


  ■ macOS

    1. Copy "Mollusk Voice Changer.vst3" to:

         ~/Library/Audio/Plug-Ins/VST3/
         (or /Library/Audio/Plug-Ins/VST3/ for all users)

    2. Rescan plugins in your DAW.

    3. Insert the plugin on a mono input track
       (e.g. a microphone channel).

    [!] Gatekeeper Notice
        This plugin is not code-signed. macOS may block it
        on first launch. If so, please allow it manually:

          System Settings → Privacy & Security
          → Click "Open Anyway" or "Allow"


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
    All parameters support DAW automation.

  ■ Preset Export / Import
    Save and load your settings as XML files via
    "EXPORT PRESET" / "IMPORT PRESET".

  ■ DAW Session Restore
    Settings are saved with your DAW project and
    restored automatically when you reopen it.


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

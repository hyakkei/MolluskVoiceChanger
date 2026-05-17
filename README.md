# Mollusk Voice Changer

声を軟体生物（イカ・タコ）風にリアルタイム加工する、通話・配信向けボイスチェンジャーです。

---

## スクリーンショット

| Basic モード | Advanced モード |
|:---:|:---:|
| ![Basic](docs/Screenshot_Basic.png) | ![Advanced](docs/Screenshot_Advanced.png) |

---

## 特徴

- **4種のプリセット** — Squid Girl / Squid Boy / Octopus Girl / Octopus Boy
- **Basic モード** — ロータリーノブで主要パラメータを直感的に操作
  - INPUT / OUTPUT レベルメーターをリアルタイム表示
- **Advanced モード** — 各エフェクトを個別に細かく調整（縦スクロール対応）
- **DSP パイプライン**
  - Noise Gate → High Pass Filter → Pitch Shifter → Formant Shifter
  - LFO Filter → Phaser → Band Pass Filter → Output Gain
- **低レイテンシ パススルー** — PITCH / BRIGHTNESS が 0 のとき FFT 処理をスキップし、遷移時はクロスフェードで滑らかに切り替え
- **セッション保持** — 終了時に設定を自動保存し、次回起動時に復元
- **プリセット エクスポート / インポート** — XML 形式で設定を保存・共有

---

## 動作環境

| 項目 | 要件 |
|------|------|
| OS | Windows 10 / 11 / macOS / Linux (glibc 2.35以上) |
| 仮想オーディオデバイス(Win) | [VB-CABLE](https://vb-audio.com/Cable/) |
| 仮想オーディオデバイス(Mac) | [BlackHole](https://existential.audio/blackhole/) |
| 仮想オーディオデバイス(Linux) | JACK / PipeWire |
| 通話/配信アプリ | 入力デバイスに 仮想オーディオデバイス を設定 |


---

## ビルド方法

### 必要なもの

- CMake 3.22 以上
- Visual Studio 2022（Windows）または Xcode（macOS）
- Git
- Linux の場合: Ubuntu 22.04 LTS 以上を推奨（glibc 2.35 以上の環境でビルドすると互換性が高い）

### Windows / macOS

```bash
# リポジトリをクローン
git clone https://github.com/hyakkei/MolluskVoiceChanger.git
cd MolluskVoiceChanger

# JUCE サブモジュールを取得
git submodule update --init --recursive

# ビルド
cmake -B build
cmake --build build --config Release
```

ビルド成果物は `build/MolluskVoiceChanger_artefacts/Release/` に生成されます。

### Linux

```bash
# リポジトリをクローン
git clone https://github.com/hyakkei/MolluskVoiceChanger.git
cd MolluskVoiceChanger

# JUCE サブモジュールを取得
git submodule update --init --recursive

# ビルド（依存パッケージの自動インストールを含む）
bash scripts/build-linux.sh

# 配布用パッケージの生成（AppImage・VST3アーカイブ）（任意）
bash scripts/package-linux.sh
```

ビルド成果物は `build-linux/` に、配布用パッケージは `release/linux/` に生成されます。

> **互換性について**: 幅広い Linux 環境への配布には Ubuntu 22.04 LTS 以上でのビルドを推奨します。
> 生成される AppImage は glibc 2.35 以上の環境であればディストリビューションを問わず動作します。

---

## セットアップ

1. **仮想オーディオデバイス** をインストールし、PCを再起動します。
2. アプリを起動し、**AUDIO SETTINGS** から以下を設定します。
   - 入力デバイス: マイク
   - 出力デバイス: 仮想オーディオデバイス Input
3. 通話/配信アプリ の設定 入力デバイスを **仮想オーディオデバイス Output** に変更します。

---

## 使い方

### Basic モード

画面上部に INPUT（エフェクト前）と OUTPUT（エフェクト後）のレベルメーターを表示します。

| ノブ | 説明 |
|------|------|
| INPUT SENSITIVITY | ノイズゲートのしきい値 |
| PITCH | ピッチシフト量（半音単位） |
| BRIGHTNESS | フォルマントシフト量 |
| BUBBLE | LFO フィルターの Dry/Wet |
| SLIMY | フェイザーの Dry/Wet |
| LO-FI | バンドパスフィルターの Dry/Wet |
| VOLUME | 出力ゲイン |

### Advanced モード

各エフェクトセクションを個別に調整できます。BYPASS チェックボックスで各エフェクトを無効化可能です。

---

## 使用ライブラリ

- [JUCE](https://juce.com/) — © Raw Material Software Limited
- フォント (Google Fonts, SIL OFL):
  - [Bungee](https://fonts.google.com/specimen/Bungee)
  - [RocknRoll One](https://fonts.google.com/specimen/RocknRoll+One)
  - [Alfa Slab One](https://fonts.google.com/specimen/Alfa+Slab+One)
  - [Rubik Glitch](https://fonts.google.com/specimen/Rubik+Glitch)

---

## ライセンス

Copyright (C) 2026 Ito Hyakkei

本ソフトウェアは [GNU Affero General Public License v3.0](LICENSE) のもとで公開されています。
ソースコードの複製・改変・再配布は、同ライセンスの条件に従う限り自由に行えます。
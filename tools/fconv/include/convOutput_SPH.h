#ifndef _CONVOUTPUT_SPH_H_
#define _CONVOUTPUT_SPH_H_

/*
 * fconv (File Converter)
 *
 * CDMlib - Cartesian Data Management library
 *
 * Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/**
 * @file   convOutput_SPH.h
 * @brief  convOutput_SPH Class Header
 * @author aics
 * @date   2013/11/7
 */

#include "convOutput.h"

class convOutput_SPH : public convOutput {

public:

  /** コンストラクタ */
  convOutput_SPH();

  /** デストラクタ */
  ~convOutput_SPH();

public:


  /**
   * @brief 出力ファイルをオープンする
   * @param [in] prefix ファイル接頭文字
   * @param [in] step ステップ数
   * @param [in] id   ランク番号
   * @param [in] mio    出力時の分割指定　 true = local / false = gather(default)
   */
  FILE* OutputFile_Open(
                        const std::string prefix,
                        const unsigned step,
                        const int id,
                        const bool mio);
                  

  /**
   * @brief sphファイルのheaderの書き込み
   * @param[in] step    ステップ数
   * @param[in] dim     変数の個数
   * @param[in] d_type  データ型タイプ
   * @param[in] imax    x方向ボクセルサイズ
   * @param[in] jmax    y方向ボクセルサイズ
   * @param[in] kmax    z方向ボクセルサイズ
   * @param[in] time    時間
   * @param[in] org     原点座標
   * @param[in] pit     ピッチ
   * @param[in] prefix  ファイル接頭文字
   * @param[in] fp      出力ファイルポインタ
   */
  bool
  WriteHeaderRecord(int step,
                    int dim,
                    CDM::E_CDM_DTYPE d_type,
                    int imax,
                    int jmax,
                    int kmax,
                    double time,
                    double* org,
                    double* pit,
                    const std::string prefix,
                    FILE *fp); 

  /**
   * @brief マーカーの書き込み
   * @param[in] dmy マーカー
   * @param[in] fp  出力ファイルポインタ 
   * @param[in] out plot3d用
   */
  bool
  WriteDataMarker(int dmy, FILE* fp, bool out);

protected:

};

#endif // _CONVOUTPUT_SPH_H_ 

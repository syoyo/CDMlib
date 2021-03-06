#ifndef _CDM_DFI_INLINE_H_
#define _CDM_DFI_INLINE_H_

/*
 * CDMlib - Cartesian Data Management library
 *
 * Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 */

/** 
 * @file   cdm_DFI.h
 * @brief  cdm_DFI Class Header
 * @author aics    
 */

#include "cdm_DFI_SPH.h"
#include "cdm_DFI_BOV.h"
#include "cdm_DFI_AVS.h"
#include "cdm_DFI_PLOT3D.h"
#include "cdm_DFI_VTK.h"
#include "cdm_NonUniformDomain.h"
#include <typeinfo>

#ifdef CDM_INLINE
 #undef CDM_INLINE
#endif

#ifndef CDM_NO_INLINE
 #define CDM_INLINE inline
#else
 #define CDM_INLINE
#endif

// #################################################################
// フィールドデータの読込み(読み込んだデータのポインタを戻り値として
// 返す）

//template<class T, class TimeT, class TimeAvrT> 
//CDM_INLINE T*
template<class TimeT, class TimeAvrT> 
CDM_INLINE void*
cdm_DFI::ReadData(CDM::E_CDM_ERRORCODE &ret,
                  const unsigned step, 
                  const int gc, 
                  const int Gvoxel[3], 
                  const int Gdivision[3], 
                  const int head[3], 
                  const int tail[3],
                  TimeT &time,
                  const bool mode, 
                  unsigned &step_avr, 
                  TimeAvrT &time_avr)
{

   int sz[3];
   for(int i=0; i<3; i++) sz[i]=tail[i]-head[i]+1;
   cdm_Array *data = cdm_Array::instanceArray
                     ( DFI_Finfo.DataType
                     , DFI_Finfo.ArrayShape
                     , sz
                     , gc
                     , DFI_Finfo.NumVariables);

   double d_time = (double)time;
   double d_time_avr = (double)time_avr;

//   int ret = ReadData(data, step, gc, Gvoxel, Gdivision, head, tail,
   ret = ReadData(data, step, gc, Gvoxel, Gdivision, head, tail,
                       d_time, mode, step_avr, d_time_avr);

   if( ret != CDM::E_CDM_SUCCESS ) {
     delete data;
     return NULL;
   }

//   T* ptr = (T*)data->getData(true);
   void* ptr = data->getData(true);
   delete data;
   time = d_time;
   time_avr = d_time_avr;

   return ptr;
}

// #################################################################
// フィールドデータの読込み(引数で渡された配列にデータを読込む）
template<class T, class TimeT, class TimeAvrT> 
CDM_INLINE
CDM::E_CDM_ERRORCODE cdm_DFI::ReadData(T *val,
                                       const unsigned step,
                                       const int gc,
                                       const int Gvoxel[3],
                                       const int Gdivision[3],
                                       const int head[3],
                                       const int tail[3],
                                       TimeT &time,
                                       const bool mode,
                                       unsigned &step_avr,
                                       TimeAvrT &time_avr)
{

   int sz[3];
   for(int i=0; i<3; i++) sz[i]=tail[i]-head[i]+1;

   cdm_Array *data = cdm_Array::instanceArray
                     ( val
                     , DFI_Finfo.ArrayShape
                     , sz
                     , gc
                     , DFI_Finfo.NumVariables);

   double d_time = (double)time;
   double d_time_avr = (double)time_avr;

   CDM::E_CDM_ERRORCODE ret;
   ret = ReadData(data, step, gc, Gvoxel, Gdivision, head, tail,
                  d_time, mode, step_avr, d_time_avr);

   if( ret == CDM::E_CDM_SUCCESS ) {
     time = d_time;
     time_avr = d_time_avr;
   }

   //data->getData(true);
   delete data;

   return ret; 
}

// #################################################################
// フィールドデータの出力
template<class T, class TimeT, class TimeAvrT> 
CDM_INLINE
CDM::E_CDM_ERRORCODE
cdm_DFI::WriteData(const unsigned step, 
                   TimeT time, 
                   const int sz[3],
                   const int nVari,
                   const int gc, 
                   T* val, 
                   T* minmax,
                   const bool avr_mode, 
                   const unsigned step_avr, 
                   TimeAvrT time_avr)
{
  //フィールドデータの変数の個数と登録された変数名の個数の一致確認
  if ( DFI_Finfo.NumVariables != DFI_Finfo.VariableName.size()) {
    printf("\tError : Number of valiable names %d %d\n", DFI_Finfo.NumVariables, DFI_Finfo.VariableName.size());
    return CDM::E_CDM_ERROR_UNMATCH_NUM_OF_VARIABLES;
  }

  cdm_Array *data = cdm_Array::instanceArray
                    ( val
                    , DFI_Finfo.ArrayShape
                    , DFI_Process.RankList[m_RankID].VoxelSize[0]
                    , DFI_Process.RankList[m_RankID].VoxelSize[1]
                    , DFI_Process.RankList[m_RankID].VoxelSize[2]
                    , gc
                    , DFI_Finfo.NumVariables);

  double d_time = (double)time;
  double d_time_avr = (double)time_avr;
  double *d_minmax=NULL;
  int num_minmax;
  if( minmax ) {
    if( DFI_Finfo.FileFormat == CDM::E_CDM_FMT_SPH && DFI_Finfo.NumVariables == 3 ) {
      num_minmax = DFI_Finfo.NumVariables*2+2;
    } else {
      num_minmax = DFI_Finfo.NumVariables*2;
    }
    d_minmax = new double[num_minmax];
    for(int i=0; i<num_minmax; i++) {
      d_minmax[i] = minmax[i];
    }
  }

  CDM::E_CDM_ERRORCODE ret;
  ret = WriteData(step, gc, d_time, data, d_minmax, avr_mode, step_avr, d_time_avr);

  //val = (T*)data->getData(true);
  //data->getData(true);

  if( d_minmax ) delete [] d_minmax;

  delete data;
  return ret;
                                          
}

// #################################################################
// フィールドデータの出力(dfi fileの出力なし)
template<class T, class TimeT, class TimeAvrT> 
CDM_INLINE
CDM::E_CDM_ERRORCODE
cdm_DFI:: WriteFieldDataFile(const unsigned step,
                             TimeT time,
                             const int sz[3],
                             const int nVari,
                             const int gc,
                             T* val,
                             const bool avr_mode,
                             const unsigned step_avr,
                             TimeAvrT time_avr)
{
  //フィールドデータの変数の個数と登録された変数名の個数の一致確認
  if ( DFI_Finfo.NumVariables != DFI_Finfo.VariableName.size()) {
    printf("\tError : Number of valiable names %d %d\n", DFI_Finfo.NumVariables, DFI_Finfo.VariableName.size());
    return CDM::E_CDM_ERROR_UNMATCH_NUM_OF_VARIABLES;
  }

  cdm_Array *data = cdm_Array::instanceArray
                    ( val
                    , DFI_Finfo.ArrayShape
                    , DFI_Process.RankList[m_RankID].VoxelSize[0]
                    , DFI_Process.RankList[m_RankID].VoxelSize[1]
                    , DFI_Process.RankList[m_RankID].VoxelSize[2]
                    , gc
                    , DFI_Finfo.NumVariables);

  double d_time = (double)time;
  double d_time_avr = (double)time_avr;

  CDM::E_CDM_ERRORCODE ret;
  ret = WriteFieldDataFile(step, gc, d_time, data, avr_mode, step_avr, d_time_avr);

  delete data;
  return ret;

}

// #################################################################
// TimeSliceをセット
template<class T, class TimeT, class TimeAvrT> 
CDM_INLINE
void 
cdm_DFI::AddTimeSlice(const unsigned step,
                      TimeT time,
                      T* minmax,
                      bool avr_mode,
                      unsigned step_avr,
                      TimeAvrT time_avr)
{

  double d_time = (double)time;
  double d_time_avr = (double)time_avr;
  double *d_minmax=NULL;
  int num_minmax;
  if( minmax ) {
    if( DFI_Finfo.FileFormat == CDM::E_CDM_FMT_SPH && DFI_Finfo.NumVariables == 3 ) {
      num_minmax = DFI_Finfo.NumVariables*2+2;
    } else {
      num_minmax = DFI_Finfo.NumVariables*2;
    }
    d_minmax = new double[num_minmax];
    for(int i=0; i<num_minmax; i++) {
      d_minmax[i] = minmax[i];
    }
  }

  //Slice へのセット
  DFI_TimeSlice.AddSlice(step, d_time, d_minmax, DFI_Finfo.NumVariables, DFI_Finfo.FileFormat,
                         avr_mode, step_avr, d_time_avr);

  if( d_minmax ) delete [] d_minmax;

}

// #################################################################
//セル中心データを 格子点にセット
template<class T1, class T2>
CDM_INLINE
bool
cdm_DFI::setGridData(cdm_TypeArray<T1>* P,
                     cdm_TypeArray<T2>* S)
{

  if( P->getArrayShape() != S->getArrayShape() ) return false;

  //変数の個数をセット
  if( P->getNvariInt() != S->getNvariInt() ) return false;
  int nVari = P->getNvariInt();

  //S(セル中心）の配列サイズを取得セット
  //T2* data = S->getData();
  const int* size = S->getArraySizeInt();
  int ix = size[0];
  int jx = size[1];
  int kx = size[2];

  //P（格子点）の配列サイズを取得セット
  T1* d    = P->getData();
  const int* Psz = P->getArraySizeInt();
  int id = Psz[0];
  int jd = Psz[1];
  int kd = Psz[2];

  //Pの配列をゼロクリア
  size_t dsize = (size_t)(id*jd*kd*nVari);
  for (size_t l=0; l<dsize; l++) d[l]=0.0;

  //S(セル中心）のデータをP(格子点)に加える
  //NIJKの処理
  if( P->getArrayShape() == CDM::E_CDM_NIJK ) {
    for (int km=0; km<kx; km++) {
    for (int jm=0; jm<jx; jm++) {
    for (int im=0; im<ix; im++) {
    for (int n=0; n<nVari; n++) {
      P->val(n, im  ,jm  ,km  ) = P->val(n, im  ,jm  ,km  )+S->val(n, im,jm,km); ///<0,0,0>
      P->val(n, im+1,jm  ,km  ) = P->val(n, im+1,jm  ,km  )+S->val(n, im,jm,km); ///<1,0,0>
      P->val(n, im+1,jm  ,km+1) = P->val(n, im+1,jm  ,km+1)+S->val(n, im,jm,km); ///<1,0,1>
      P->val(n, im  ,jm  ,km+1) = P->val(n, im  ,jm  ,km+1)+S->val(n, im,jm,km); ///<0,0,1>
      P->val(n, im  ,jm+1,km  ) = P->val(n, im  ,jm+1,km  )+S->val(n, im,jm,km); ///<0,1,0>
      P->val(n, im+1,jm+1,km  ) = P->val(n, im+1,jm+1,km  )+S->val(n, im,jm,km); ///<1,1,0>
      P->val(n, im+1,jm+1,km+1) = P->val(n, im+1,jm+1,km+1)+S->val(n, im,jm,km); ///<1,1,1>
      P->val(n, im  ,jm+1,km+1) = P->val(n, im  ,jm+1,km+1)+S->val(n, im,jm,km); ///<0,1,1>
    }}}}
  } else {
  //IJKNの処理
    for (int n=0; n<nVari; n++) {
    for (int km=0; km<kx; km++) {
    for (int jm=0; jm<jx; jm++) {
    for (int im=0; im<ix; im++) {
      P->val(im  ,jm  ,km  ,n) = P->val(im  ,jm  ,km  ,n)+S->val(im,jm,km,n); ///<0,0,0>
      P->val(im+1,jm  ,km  ,n) = P->val(im+1,jm  ,km  ,n)+S->val(im,jm,km,n); ///<1,0,0>
      P->val(im+1,jm  ,km+1,n) = P->val(im+1,jm  ,km+1,n)+S->val(im,jm,km,n); ///<1,0,1>
      P->val(im  ,jm  ,km+1,n) = P->val(im  ,jm  ,km+1,n)+S->val(im,jm,km,n); ///<0,0,1>
      P->val(im  ,jm+1,km  ,n) = P->val(im  ,jm+1,km  ,n)+S->val(im,jm,km,n); ///<0,1,0>
      P->val(im+1,jm+1,km  ,n) = P->val(im+1,jm+1,km  ,n)+S->val(im,jm,km,n); ///<1,1,0>
      P->val(im+1,jm+1,km+1,n) = P->val(im+1,jm+1,km+1,n)+S->val(im,jm,km,n); ///<1,1,1>
      P->val(im  ,jm+1,km+1,n) = P->val(im  ,jm+1,km+1,n)+S->val(im,jm,km,n); ///<0,1,1>
    }}}}
  }

  //内部の格子点のデータを重み付けで割る
  VolumeDataDivide(P);

  return true;

}

// #################################################################
//内部の格子点のデータを重み付けで割る
template<class T>
CDM_INLINE
void
cdm_DFI::VolumeDataDivide(cdm_TypeArray<T> *P)
{
  int i,j,k,n;
  const int* szP = P->getArraySizeInt();
  int id = szP[0];
  int jd = szP[1];
  int kd = szP[2];

  int nvari = P->getNvariInt();

  //NIJK
  if( P->getArrayShape() == CDM::E_CDM_NIJK ) {

    //I
    for (k=0; k<kd;    k++){
    for (j=0; j<jd;    j++){
    for (i=1; i<id-1;  i++){
    for (n=0; n<nvari; n++){
      P->val(n,i,j,k) = P->val(n,i,j,k)*0.5;
    }}}}

    //J
    for (k=0; k<kd;    k++){
    for (j=1; j<jd-1;  j++){
    for (i=0; i<id;    i++){
    for (n=0; n<nvari; n++){
      P->val(n,i,j,k) = P->val(n,i,j,k)*0.5;
    }}}}

    //K
    for (k=1; k<kd-1;  k++){
    for (j=0; j<jd;    j++){
    for (i=0; i<id;    i++){
    for (n=0; n<nvari; n++){
      P->val(n,i,j,k) = P->val(n,i,j,k)*0.5;
    }}}}

  //IJKN
  } else {

    //I
    for (n=0; n<nvari; n++){
    for (k=0; k<kd;    k++){
    for (j=0; j<jd;    j++){
    for (i=1; i<id-1;  i++){
      P->val(i,j,k,n) = P->val(i,j,k,n)*0.5;
    }}}}

    //J
    for (n=0; n<nvari; n++){
    for (k=0; k<kd;    k++){
    for (j=1; j<jd-1;  j++){
    for (i=0; i<id;    i++){
      P->val(i,j,k,n) = P->val(i,j,k,n)*0.5;
    }}}}

    //K
    for (n=0; n<nvari; n++){
    for (k=1; k<kd-1;  k++){
    for (j=0; j<jd;    j++){
    for (i=0; i<id;    i++){
      P->val(i,j,k,n) = P->val(i,j,k,n)*0.5;
    }}}}

  }
}

// #################################################################
// DFI Write インスタンス template 型 (等間隔格子用)
template<typename T>
CDM_INLINE
cdm_DFI* cdm_DFI::WriteInit(const MPI_Comm comm,
                            const std::string DfiName,
                            const std::string Path,
                            const std::string prefix,
                            const CDM::E_CDM_FORMAT format,
                            const int GCell,
                            const CDM::E_CDM_DTYPE DataType,
                            const int nVari,
                            const std::string proc_fname,
                            const int G_size[3],
                            const T pitch[3],
                            const T L_origin[3],
                            const int division[3],
                            const int head[3],
                            const int tail[3],
                            const std::string hostname,
                            const CDM::E_CDM_ONOFF TSliceOnOff)
{

  //インスタンスout_domainを生成し、等間隔格子・不等間隔格子の共通処理版のWriteInit関数を呼ぶ

  //AVS,PLOT3D,VTK: cdm_DomainのGlobalOriginには、計算領域全体の原点座標値を与える。
  //SPH,BOV: cdm_DomainのGlobalOriginには、各ランクの局所領域における原点座標値を与える。
  T G_origin[3];
  if( format == CDM::E_CDM_FMT_AVS || format == CDM::E_CDM_FMT_PLOT3D || format == CDM::E_CDM_FMT_VTK) {
    for(int i=0; i<3; i++) G_origin[i] = L_origin[i] - ((T)(head[i]-1))*pitch[i];
  } else {
    for(int i=0; i<3; i++) G_origin[i] = L_origin[i];
  }

  cdm_Domain* out_domain = NULL;
  out_domain = new cdm_Domain(G_origin,
                              pitch,
                              G_size,
                              division);

  return WriteInit<T>(comm,
                      DfiName,
                      Path,
                      prefix,
                      format,
                      GCell,
                      DataType,
                      nVari,
                      proc_fname,
                      out_domain,
                      head,
                      tail,
                      hostname,
                      TSliceOnOff);

}

// #################################################################
// DFI Write インスタンス template 型 (不等間隔格子用)
template<typename T>
CDM_INLINE
cdm_DFI* cdm_DFI::WriteInit(const MPI_Comm comm,
                            const std::string DfiName,
                            const std::string Path,
                            const std::string prefix,
                            const CDM::E_CDM_FORMAT format,
                            const int GCell,
                            const CDM::E_CDM_DTYPE DataType,
                            const int nVari,
                            const std::string proc_fname,
                            const int G_size[3],
                            const T* coord_X,
                            const T* coord_Y,
                            const T* coord_Z,
                            const std::string coord_file,
                            const CDM::E_CDM_FILE_TYPE coord_filetype,
                            const CDM::E_CDM_ENDIANTYPE coord_fileEndian,
                            const int division[3],
                            const int head[3],
                            const int tail[3],
                            const std::string hostname,
                            const CDM::E_CDM_ONOFF TSliceOnOff)
{

  //インスタンスout_domainを生成し、等間隔格子・不等間隔格子の共通処理版のWriteInit関数を呼ぶ

  //Check format
  if( format == CDM::E_CDM_FMT_SPH ) {
    printf("\tCDM error : Non_Uniform_Cartesian is not supported in SPH File Format.\n");
    return NULL;
  } else if ( format == CDM::E_CDM_FMT_BOV ) {
    printf("\tCDM error : Non_Uniform_Cartesian is not supported in BOV File Format.\n");
    return NULL;
  }

  //Check pointer to coordinates
  if ( coord_X == NULL || coord_Y == NULL || coord_Z == NULL ) {
    printf("\tCDM error : Pointer to coordinates is not set.\n");
    return NULL;
  }

  //Check of extension of CoordinateFile
  string ext;
  size_t pos_dot = coord_file.rfind('.');
  if(pos_dot != string::npos){
    ext = coord_file.substr(pos_dot+1, coord_file.size()-pos_dot);
  } else {
    printf("\tFail to get extension of '%s'\n",coord_file.c_str());
    return NULL;
  }
  if(ext != "crd"){
    printf("\tCDM error : Extension of Coordinate File '%s' is not 'crd'. \n",ext.c_str());
    return NULL;
  }

  cdm_Domain* out_domain = NULL;
  out_domain = new cdm_NonUniformDomain<T>(coord_X,
                                           coord_Y,
                                           coord_Z,
                                           coord_file,
                                           coord_filetype,
                                           coord_fileEndian,
                                           G_size,
                                           division,
                                           GCell);

  return WriteInit<T>(comm,
                      DfiName,
                      Path,
                      prefix,
                      format,
                      GCell,
                      DataType,
                      nVari,
                      proc_fname,
                      out_domain,
                      head,
                      tail,
                      hostname,
                      TSliceOnOff);

}

// #################################################################
// DFI Write インスタンス template 型 (等間隔格子・不等間隔格子の共通処理部分)
template<typename T>
CDM_INLINE
cdm_DFI* cdm_DFI::WriteInit(const MPI_Comm comm,
                            const std::string DfiName,
                            const std::string Path,
                            const std::string prefix,
                            const CDM::E_CDM_FORMAT format,
                            const int GCell,
                            const CDM::E_CDM_DTYPE DataType,
                            const int nVari,
                            const std::string proc_fname,
                            const cdm_Domain* out_domain,
                            const int head[3],
                            const int tail[3],
                            const std::string hostname,
                            const CDM::E_CDM_ONOFF TSliceOnOff)
{

//Check for SPH format 20141022.s
  if( format == CDM::E_CDM_FMT_SPH ) {
    if( nVari != 1 && nVari != 3 ) {
      printf("\tCDM error sph file undefined except for number of valiables 1 or 3.\n");
      return NULL;
    }
  }
//Check for SPH format 20141022.e

  cdm_DFI *dfi = NULL;

  int RankID;
  MPI_Comm_rank( comm, &RankID );

  int nrank;
  MPI_Comm_size( comm, &nrank );

  cdm_FileInfo out_F_info;
  if( typeid(*out_domain) == typeid(cdm_Domain) ){
    out_F_info.DFIType = CDM::E_CDM_DFITYPE_CARTESIAN;
  } else if( typeid(*out_domain) == typeid(cdm_NonUniformDomain<T>) ) {
    out_F_info.DFIType = CDM::E_CDM_DFITYPE_NON_UNIFORM_CARTESIAN;
  } else {
    printf("\tCDM error : Can't get DFIType.\n");
    return NULL;
  }
  out_F_info.DirectoryPath    = Path;
  out_F_info.TimeSliceDirFlag = TSliceOnOff;
  out_F_info.Prefix           = prefix;
  out_F_info.FileFormat       = format;
  out_F_info.GuideCell        = GCell;
  out_F_info.DataType         = DataType;
  if( format == CDM::E_CDM_FMT_BOV || format == CDM::E_CDM_FMT_PLOT3D ) {
    out_F_info.ArrayShape = CDM::E_CDM_IJKN;
  }
  else if( format == CDM::E_CDM_FMT_SPH || format == CDM::E_CDM_FMT_AVS || format == CDM::E_CDM_FMT_VTK) {
    out_F_info.ArrayShape = CDM::E_CDM_NIJK;
  }
  out_F_info.NumVariables     = nVari;

  int idumy = 1;
  char* cdumy = (char*)(&idumy);
  if( cdumy[0] == 0x01 ) out_F_info.Endian = CDM::E_CDM_LITTLE;
  if( cdumy[0] == 0x00 ) out_F_info.Endian = CDM::E_CDM_BIG;

  cdm_FilePath out_F_path;
  out_F_path.ProcDFIFile = proc_fname;

  cdm_VisIt out_visit;
  out_visit.PlotGC = "off";
  
  cdm_Unit out_unit;

  cdm_MPI out_mpi;
  out_mpi.NumberOfRank = nrank;
  out_mpi.NumberOfGroup = 1;

  cdm_Process out_Process;
  cdm_Rank out_Rank;

  for(int i=0; i<nrank; i++ ) {
     out_Process.RankList.push_back(out_Rank);
  }

  out_Process.RankList[RankID].RankID=RankID;
  out_Process.RankList[RankID].HostName=hostname;
  for(int i=0; i<3; i++) {
    out_Process.RankList[RankID].HeadIndex[i]=head[i];
    out_Process.RankList[RankID].TailIndex[i]=tail[i];
    out_Process.RankList[RankID].VoxelSize[i]=tail[i]-head[i]+1;
  }

  cdm_TimeSlice out_TSlice;

  char tmpname[512];
  memset(tmpname,0x00,sizeof(char)*512);
  if( gethostname(tmpname, 512) != 0 ) printf("*** error gethostname() \n");

  if( out_F_info.FileFormat == CDM::E_CDM_FMT_SPH ) {
    dfi = new cdm_DFI_SPH(out_F_info, out_F_path, out_visit, out_unit, out_domain, out_mpi,
                          out_TSlice, out_Process);
  } else if( out_F_info.FileFormat == CDM::E_CDM_FMT_BOV ) {
    dfi = new cdm_DFI_BOV(out_F_info, out_F_path, out_visit, out_unit, out_domain, out_mpi,
                          out_TSlice, out_Process);
//FCONV 20131122.s
  } else if( out_F_info.FileFormat == CDM::E_CDM_FMT_AVS ) {
    dfi = new cdm_DFI_AVS(out_F_info, out_F_path, out_visit, out_unit, out_domain, out_mpi,
                          out_TSlice, out_Process);
  } else if( out_F_info.FileFormat == CDM::E_CDM_FMT_PLOT3D ) {
    dfi = new cdm_DFI_PLOT3D(out_F_info, out_F_path, out_visit, out_unit, out_domain, out_mpi,
                          out_TSlice, out_Process);
  } else if( out_F_info.FileFormat == CDM::E_CDM_FMT_VTK ) {
    dfi = new cdm_DFI_VTK(out_F_info, out_F_path, out_visit, out_unit, out_domain, out_mpi,
                          out_TSlice, out_Process);
//FCONV 20131122.e
  } else return NULL;


  dfi->m_indexDfiName = DfiName;
  dfi->m_directoryPath = CDM::cdmPath_DirName(DfiName);
  dfi->m_comm = comm;
  dfi->m_RankID = RankID;

  return dfi;

}

// #################################################################
// コンパイルオプションで与えたバッファサイズを取得する
CDM_INLINE
int cdm_DFI::getBufSize()
{
#ifdef CDM_BUFFER_MB_SIZE
  return CDM_BUFFER_MB_SIZE;
#else
  return 0;
#endif
}

#endif // _CDM_DFI_INLINE_H_

function please_mex_me

sCurrentPath = cd;
sPath = fileparts(mfilename('fullpath'));
cd(sPath)
mex([sPath, filesep, 'fLiveWireCalcP.cpp']);


cd(sCurrentPath);

for %%a in (.) do set ProjectName=%%~nxa
"\program files\7-zip\7z" a -r -x!*.idb -x!*.res -x!*.exe -x!*.db -x!*.sdf -x!*.suo -x!*.user -x!*.aps -x!*.ncb -x!*.obj -x!*.pch -x!*.ipch -x!*.lib -x!*.zip -x!*.ilk -x!*.tlog -x!*.pdb -x!*.dll %ProjectName%.zip *.*

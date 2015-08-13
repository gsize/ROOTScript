void list_files(TString dirname="./", const TString ext=".C")
{
TString dirPath = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
	dirPath.ReplaceAll("list_files.C","");
	dirPath.ReplaceAll("/./","/");
	
	dirname = dirPath;
   TSystemDirectory dir;
   dir.SetDirectory(dirname.Data());
   TList *files = dir.GetListOfFiles();
   if (files) {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while ((file=(TSystemFile*)next())) {
         fname = file->GetName();
         if (!file->IsDirectory() && fname.EndsWith(ext.Data())) {
            cout << fname.Data() << endl;
         }
      }
   }
}

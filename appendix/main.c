#include "all.h"
#include "jpeg/all.h"
#include "pdf/all.h"
#include "getsubstr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include<sys/time.h>
#include "media/mp3parse.h"
#include "media/mp4parse.h"
#include "office/officeparser.h"


static void DealFile(char* filename,char* tpaths);
static int uncompress(char *compressedFile,char* srcpath,char* paths);
static int txtparser(char *srcfile, char* workspace, char *destpath);

static char * worksp=NULL;
//#define __DEBUG 
int AppendixMain(int argc, char* argv[])
{
    static int flags=0;
    char oldpath[1024]= {0};
    int iiiii=0;
    DIR* d;
    getcwd(oldpath,sizeof(oldpath));
    struct dirent *file;
    worksp=argv[0];
    char ptptptp[1024]="appendix";
    char zippppp[100]={0};
    if(!(d=opendir(argv[1])))
    {
        printf("error in open dir : %s\n",argv[1]);
        return -1;
    }
    //printf("add is %s\n",argv[1]);
#ifdef __DEBUG    
    printf("oldpath %s\n",oldpath);
#endif

    while((file=readdir(d))!=NULL)
    {
    	//printf("file->d_name is %s\n",file->d_name);
        if(strncmp(file->d_name,".",1)==0)
            continue;
        {
            /*判断是文件夹处理下一个*/
            struct stat info;
            stat(file->d_name,&info);
            if(S_ISDIR(info.st_mode))
            {
#ifdef __DEBUG            
                printf("This is a directory\n");
#endif                
                continue;
            }
        } 
        sprintf(zippppp,"ziptemps%d",iiiii++);
        uncompress(file->d_name,ptptptp,zippppp);       
    }
    closedir(d);
    return 0;
}
extern int PdfMain(int argc, char * argv[]);
extern int JpegMain(int argc, char * argv[]);
extern int ZipsMain(int argc, char * argv[]);

//#define __DEBUG
static void DealFile(char* filename,char* tpaths)
{
    char *supports[]= {"doc","docx","ppt","pptx","xls","xlsx","pdf","jpeg","jpg","mp3","mp4","txt"};
    enum supportType {DOC,DOCX,PPT,PPTX,XLS,XLSX,PDF,JPEG,JPG,MP3,MP4,TXT,OTHERS} FileType;
    int index, NType;
    char *suffix=NULL;
    char *inputs[5]= {0};
    int inputNum=0;
    char abspath[1024]={0};
    getcwd(abspath,sizeof(abspath));
    sprintf(abspath,"%s/%s",abspath,worksp);
    NType=sizeof(supports)/sizeof(supports[0]);
    suffix=GetSuffix(filename);
    if(suffix)
    {
        for(index=0; index<NType; index++)
        {
            if(!strcmp(supports[index],suffix))
                break;
        }
        FileType=(enum supportType)index;
    }
    else
    {
        FileType=OTHERS;
    }
    switch(FileType)
    {
    case DOC:
#ifdef __DEBUG
        printf("deal with doc file\n");
#endif
 //       break;
    case DOCX:
#ifdef __DEBUG    
        printf("deal with docx file\n");
#endif    
		officeparser(filename, abspath,tpaths,"test.email.txt");
        break;
    case PPT:
#ifdef __DEBUG    
        printf("deal with ppt file\n");
#endif        
//        break;
    case PPTX:
#ifdef __DEBUG    
        printf("deal with pptx file\n");
#endif  
		officeparser(filename, abspath,tpaths,"test.email.txt");     
        break;
    case XLS:
#ifdef __DEBUG    
        printf("deal with xls file\n");
#endif  
    case XLSX:
#ifdef __DEBUG    
        printf("deal with xlsx file\n");
#endif  
		officeparser(filename, abspath,tpaths,"test.email.txt");     
        break;
        
    case PDF:
#ifdef __DEBUG
        printf("deal with pdf file\n");
#endif  	
		//PdfParse(filename,worksp,tpaths);   
		officeparser(filename, abspath,tpaths,"test.email.txt");      
        break;
    case JPEG:
#ifdef __DEBUG    
        printf("deal with jpeg file\n");
#endif        
    case JPG:
#ifdef __DEBUG    
        printf("deal with jpg file\n");
#endif
       	JpegParse(filename, worksp, tpaths);/*worksp/tpaths/filename tpaths=appendix?*/
        break;
    case MP3:
#ifdef __DEBUG    
        printf("deal with mp3 file\n");
        audiopaser(filename,worksp,tpaths);
        break;
#endif        
    case MP4:
#ifdef __DEBUG    
        printf("deal with mp4 file\n");
#endif        
        videoparser(filename,worksp,tpaths);       
        break;
    case TXT:
#ifdef __DEBUG    
        printf("deal with TXT file\n");
#endif        
        txtparser(filename,worksp,tpaths);       
        break;
    case OTHERS:
    default:
#ifdef __DEBUG
        printf("unknow file type (.%s), make sure it valid\n",filename);
#endif        
        break;
    }
}


static int whichKindOfCompressedFile(char *compressedFile)
//0->不是压缩文件   1 ->rar  2->zip  3->tar.gz 4->tar 5->tar.bz2
{
    char *supportsKindOfCompress[] = {"rar","zip","gz","tar","bz2"};
    int NumberOfType = sizeof(supportsKindOfCompress)/sizeof(supportsKindOfCompress[0]);
    char *p = NULL;
    int isCompressed = 0;
    p = GetSuffix(compressedFile);
    if (p)
    {
        int i = 0;
        while (i < NumberOfType)
        {
            if (!strcmp(supportsKindOfCompress[i], p))
            {
                isCompressed = i + 1;
                break;
            }
            i++;
        }
    }
    if (isCompressed == 0)
    {
        return 0;
    }
    return isCompressed;
}

static int uncompress(char *compressedFile,char* srcpath,char* paths)//需要传入一个带有绝对路径的压缩文件
{
    char *commandPool[] = {"rar e -y -inul ","unzip -j -q ","tar -xzf ","tar -xf ","tar -xjf "};
    char *secondcommand[]={" "," -d "," -C "," -C "," -C "};
    char command[1024] = {0};
    int flag = whichKindOfCompressedFile(compressedFile);/*获取文件的压缩格式*/
    if (flag == 0)//源文件不是压缩文件
    {
    	DealFile(compressedFile,"appendix");
        return 1;
    }
    strncpy(command, commandPool[flag-1], strlen(commandPool[flag-1]));/*拷贝命令*/

	char ziptemp[1024]={0};
	char exes[1024]={0};
	sprintf(ziptemp,"%s/%s",worksp,paths);
	if(mkdir(ziptemp,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)!=0)
	{
		printf("error in mkdir %s\n",ziptemp);
		return -1;
	}
	
	sprintf(exes,"%s %s/%s/%s %s %s/%s",commandPool[flag-1],worksp,srcpath,compressedFile,secondcommand[flag-1],worksp,paths);
	
	//printf("%s\n",exes);
	system(exes);
	struct dirent *file;
    DIR *d;
    char curpath[1024]={0};
    sprintf(curpath,"%s/%s",worksp,paths);
    if(!(d=opendir(curpath)))
    {
        printf("error in open dir : %s\n",curpath);
        return -1;
    }
    while ((file = readdir(d)) != NULL)
    {
        if(strncmp(file->d_name,".",1)==0)
            continue;
        int tempFlag = whichKindOfCompressedFile(file->d_name);
        if (tempFlag > 0)
        {
        	char secondpath[1024]={0};
        	char firstpath[1024]={0};
        	strcat(firstpath,paths);
        	sprintf(secondpath,"%s/ziptemps",paths);
            uncompress(file->d_name,firstpath,secondpath);
        }
        else
        {
            //printf("decode this file with name :%s/%s\n",paths,file->d_name);
            DealFile(file->d_name,paths);
        }
    }
    closedir(d);
    return EXIT_SUCCESS;
}

static int txtparser(char *srcfile, char* workspace, char *destpath) 
{
	char srcpath[1024]={0};
	char respath[1024]={0};
	
	sprintf(respath,"%s/temps/test.email.txt",workspace);
	sprintf(srcpath,"%s/%s/%s",workspace,destpath,srcfile);
	printf("********************%s********************\n",srcpath);
	FILE *from=NULL, *to=NULL;
	if((from=fopen(srcpath,"rb"))!=NULL)
	{
		char* txt_ptr=NULL;
		if((to=fopen(respath,"ab"))==NULL)
		{
			printf("can't open file for decode attachment file , will give up this file\n");
			fclose(from);
			return -1;
		}
		{
			int NByte=0;
			fseek(from,0,SEEK_END);
			NByte=ftell(from);
			txt_ptr=(char*)malloc(NByte+20);
			if(txt_ptr)
			{
				memset(txt_ptr,0,20+NByte);
			}else
			{
				printf("can't malloc such huge page for loading txt\n");
				fclose(from);
				return -1;
			}
			fseek(from,0,SEEK_SET);
			fread(txt_ptr,1,NByte+20-1,from);
			fwrite(txt_ptr,1,strlen(txt_ptr),to);
			free(txt_ptr);
			txt_ptr=NULL;
		}
		fclose(to);
		fclose(from);
	}
	return 0;
}


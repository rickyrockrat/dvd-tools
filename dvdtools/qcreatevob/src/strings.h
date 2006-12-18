#include <qapplication.h>
#include <qlineedit.h>
#include <qstring.h>

#include <stdio.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>

#define MAXL 4096

#define ERROR -1

#ifndef STRINGS_H
#define STRINGS_H



QString getTempDir(QString inputFileStr)
{
  inputFileStr.replace(QRegExp("^(.*/)(.*)"), "\\1");
  return QString(inputFileStr);
}

QString getFileName(QString inputFileStr)
{
  if (inputFileStr.isEmpty())
  {
    return QString("");
  }
  else
  {
    inputFileStr.replace(QRegExp("^(.*/)(.*)[.](.*)"), "\\2");
    return QString(inputFileStr);
  }
}

QString getInputPath(QString inputFileStr)
{
  if (inputFileStr.isEmpty())
  {
    return QString(getenv("HOME"));
  }
  else
  {
    inputFileStr.replace(QRegExp("^(.*/)(.*)"), "\\1");
    return QString(inputFileStr);
  }
}

QString getScriptBegin(QString inputFileStr, QString outFileStr)
{
  QString tmp = QString().append("#####################################\n")
                .append("## ").append(inputFileStr).append("\n")
                .append("## ").append(outFileStr).append("\n")
                .append("#####################################").append("\n\n");
  return QString(tmp);

}

QString addK(QString str)
{
  QString tmp = "\"" + str + "\"";
  return QString(tmp);
}

QString getRes(QString format, QString res)
{
  QString outres;
  if (format == "pal")
  {
    outres = "720x576";
    if( res == "720x576" )
    {
      outres = "704x576";
    }

    if( res == "704x576" )
    {
      outres = "352x576";
    }

    if( res == "352x576" )
    {
      outres = "352x288";
    }
  }

  if( format == "ntsc" )
  {
    outres = "720x480";

    if( res == "720x480" )
    {
      outres = "704x480";
    }

    if( res == "704x480" )
    {
      outres = "352x480";
    }

    if( res == "352x480" )
    {
      outres = "352x240";
    }
  }
  return QString(outres);
}

QString getFps(QString format, QString fps)
{
  QString outfps;
  if( format == "ntsc" )
  {
    outfps = "23.976";
    if( fps == "23.976" )
    {
      outfps = "29.97";
    }
  }

  if( format == "pal" )
  {
    outfps = "25";
  }
  return QString(outfps);
}

QString getAudioBr(QString type)
{
  QString outBr;
  if (type == "Surround 5.1")
  {
    outBr = "384";
  }
  if (type == "Stereo 2.0")
  {
    outBr = "224";
  }
  if (type == "Mono 1.0")
  {
    outBr = "224";
  }
  if (type == "Original")
  {
    outBr = "0";
  }
  return QString(outBr);
}

QString numFilter( int i )
{
  double tmp = i;
  tmp = tmp/2;
  QString s; s.setNum(tmp);

  if ( s.find(".") != -1 )
  {
    i = i-1;
    s=s.setNum(i);
  }
  return QString( s );
}

QString getBorder(QString aspect, QString x, QString y)
{
  bool ok1, ok2;
  int WidthX=x.toInt(&ok2, 10);
  int HeightY=y.toInt(&ok1, 10);
  QString sBorder = "";
  QString sBorderTmp;

  if (ok1 && ok2)
  {
    if( aspect == "1:1" )
    {
      int border=(int)((-1)*(WidthX-HeightY)/2);
      
      sBorder = numFilter(border);
      sBorder = sBorder + "," + "0," + sBorder + "," + "0";
    }
    if( aspect == "4:3" )
    {
      int border=(int)((-1)*(0.75*WidthX-HeightY)/2);
      sBorder = numFilter(border);
      sBorder = sBorder + "," + "0," + sBorder + "," + "0";
    }
    if( aspect == "16:9" )
    {
      int border=(int)((-1)*(0.5625*WidthX-HeightY)/2);
      sBorder = numFilter(border);
      sBorder = sBorder + "," + "0," + sBorder + "," + "0";
    }
    if( aspect == "2.21:1" )
    {
      int border=(int)((-1)*(0.4524886877828054*WidthX-HeightY)/2);
      sBorder = numFilter(border);
      sBorder = sBorder + "," + "0," + sBorder + "," + "0";
    }

  }
  return QString(sBorder);
}

QString getSelBr(QString currentBr)
{
  QString borderOut;
  if( currentBr.isEmpty() )
  {
    borderOut = "0,0,0,0";
  }
  if( currentBr.find(",") == -1 && currentBr.find(" ") == -1)
  {
    borderOut = currentBr;
    borderOut.replace(QRegExp("^(.*)$"), "\\1,0,\\1,0");
  }
  if( currentBr.find(QRegExp("^(.*,)(.*)$")) != -1 )
  {
    borderOut = currentBr;
    borderOut.replace(QRegExp("^(.*,)(.*)$"), "\\1\\2,\\1\\2");
  }

  if( currentBr.find(QRegExp("^(.*,)(.*,)(.*,)(.*)$")) != -1 )
  {
    borderOut = currentBr;
    borderOut.replace(QRegExp("^(.*,)(.*,)(.*,)(.*)$"), "\\1\\2\\3\\4");
  }
  return QString(borderOut);
}

QString getResForBS(QString aspect, QString x)
{
  bool ok;
  int WidthX=x.toInt(&ok, 10);
  QString newHeightY;
  QString outResol = "";

  if (ok)
  {
    if( aspect == "1:1" )
    {
      newHeightY.setNum(WidthX);
      outResol = x + "," + newHeightY;
    }
    if( aspect == "4:3" )
    {
      int sum=(int)((0.75*WidthX));
      newHeightY.setNum(sum);
      outResol = x + "," + newHeightY;
    }
    if( aspect == "16:9" )
    {
      int sum=(int)((0.5625*WidthX));
      newHeightY.setNum(sum);
      outResol = x + "," + newHeightY;
    }
    if( aspect == "2.21:1" )
    {
      int sum=(int)((0.4524886877828054*WidthX));
      newHeightY.setNum(sum);
      outResol = x + "," + newHeightY;
    }

  }
  return QString(outResol);
}

QString getProcOutput(QString path, QString arg0, QString arg1, QString arg2)
{
  int fd[2];
  pid_t pid;
  FILE* pipe_reader;
  char buffer[MAXL];

  if(pipe(fd) == ERROR)
    return QString();

  if((pid=fork()) == ERROR)
    return QString();

  switch(pid)
  {
  case -1:
    return QString();
    break;
  case 0:
    close(fd[0]);
    if(fd[1] != STDOUT_FILENO)
    {
      if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
        return QString();
      close(fd[1]);
    }
    if(execl(path.latin1(), arg0.latin1(), arg1.latin1(), arg2.latin1(), NULL) == ERROR)
      return QString();
    break;
  default:
    sleep(1);
    close(fd[1]);

    if((pipe_reader = fdopen(fd[0], "r")) == NULL)
      return QString();

    fread(&buffer, MAXL, 1, pipe_reader);

    fclose(pipe_reader);

    printf("path = %s", buffer);

    if(waitpid(pid,NULL,0) == ERROR)
      return QString();
    break;
  }

  return QString(buffer);
}


#endif


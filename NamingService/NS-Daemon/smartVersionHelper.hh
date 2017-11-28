// --------------------------------------------------------------------------
//
//  Copyright (C) 2009 Alex Lotz
//
//        lotz@hs-ulm.de
//        schlegel@hs-ulm.de
//
//        Prof. Dr. Christian Schlegel
//        University of Applied Sciences
//        Prittwitzstr. 10
//        D-89075 Ulm
//        Germany
//
//
//  This file is part of ACE/SmartSoft.
//
//  ACE/SmartSoft is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ACE/SmartSoft is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with ACE/SmartSoft.  If not, see <http://www.gnu.org/licenses/>.
//
// --------------------------------------------------------------------------

#ifndef _VERSION_HH
#define _VERSION_HH

#include "ace/SString.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

namespace SmartACE
{

   class VersionHelper
   {
   public:

      static int get_smartsoft_version(ACE_TString &version, const ACE_TString &path="..", const ACE_TString &file="VERSION")
      {
#ifdef WIN32
         ACE_TString filename = path + "\\" + file;
#else
         ACE_TString filename = path + "/" + file;
#endif

         FILE *fp = ACE_OS::fopen (filename.c_str(),
                                  ACE_TEXT ("r"));

         if (fp == 0)
         {
            // Invalid svc.conf file.  We'll report it here and break out of
            // the method.
            if (ACE::debug ())
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("ACE (%P|%t): %s\n"),
                          filename.c_str()));

            // Use stat to find out if the file exists.  I didn't use access()
            // because stat is better supported on most non-unix platforms.
            ACE_stat exists;
            if (ACE_OS::stat (filename.c_str(), &exists) == 0) {
              // If it exists, but we couldn't open it for reading then we
              // must not have permission to read it.
              errno = EPERM;
            }else{
              errno = ENOENT;
            }

            if (ACE::debug ())
               ACE_DEBUG((LM_ERROR, ACE_TEXT("File opening failed!\n") ));
            return -1;
         }else{
            // file found and opened successfully
            if(getline(fp, version) != 0)
            {
               if (ACE::debug ())
                  ACE_DEBUG((LM_ERROR, ACE_TEXT("Error by reading %s-file.\n"), file.c_str() ));

               (void) ACE_OS::fclose (fp);
               return -1;
            }else{
               // success!
               (void) ACE_OS::fclose (fp);
               return 0;
            }
         }

         return 0;
      }


      static int getline (FILE *fp, ACE_TString &line)
      {
        int ch;

        line.set (0, 0);

        while ((ch = ACE_OS::fgetc (fp)) != EOF && ch != ACE_TEXT ('\n'))
          line += (ACE_TCHAR) ch;

        if (ch == EOF && line.length () == 0)
          return -1;
        else
          return 0;
      }

      static int get_ace_version(ACE_TString &version, const ACE_TString &env_var="ACE_ROOT", const ACE_TString &file="VERSION")
      {
         ACE_TString path = ACE_OS::getenv(env_var.c_str());
#ifdef WIN32
         ACE_TString filename = path + "\\" + file;
#else
         ACE_TString filename = path + "/" + file;
#endif

         FILE *fp = ACE_OS::fopen (filename.c_str(),
                                  ACE_TEXT ("r"));

         if (fp == 0)
         {
            // Invalid svc.conf file.  We'll report it here and break out of
            // the method.
            if (ACE::debug ())
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("ACE (%P|%t): %s\n"),
                          filename.c_str()));

            // Use stat to find out if the file exists.  I didn't use access()
            // because stat is better supported on most non-unix platforms.
            ACE_stat exists;
            if (ACE_OS::stat (filename.c_str(), &exists) == 0) {
              // If it exists, but we couldn't open it for reading then we
              // must not have permission to read it.
              errno = EPERM;
            }else{
              errno = ENOENT;
            }

            if (ACE::debug ())
               ACE_DEBUG((LM_ERROR, ACE_TEXT("File opening failed!\n") ));
            return -1;
         }else{
            // file found and opened successfully
            if(getline(fp, version) != 0)
            {
               if (ACE::debug ())
                  ACE_DEBUG((LM_ERROR, ACE_TEXT("Error by reading %s-file.\n"), file.c_str() ));

               (void) ACE_OS::fclose (fp);
               return -1;
            }else{
               // success!
               (void) ACE_OS::fclose (fp);
               return 0;
            }
         }

         return 0;
      }

      
   };


} // end namespace

#endif // _VERSION_HH

// copyright (c) 2016 Richard Guadagno
// contact: rrguadagno@gmail.com
//
// This file is part of French-Roast
//
//    French-Roast is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    French-Roast is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#include <jni.h>
#ifndef AGE_H
#define AGE_H
#ifdef __cplusplus
extern "C" {
#endif
  namespace frenchroast { namespace agent {
JNIEXPORT void JNICALL Java_java_lang_Package_thook
(JNIEnv *, jclass, jobject);

JNIEXPORT void JNICALL Java_java_lang_Package_timerhook
  (JNIEnv *, jclass, jlong, jstring, jstring);

JNIEXPORT void JNICALL Java_java_lang_Package_heaphook
  (JNIEnv *, jclass, jobject);
  
    }
  }
#ifdef __cplusplus
}
#endif
#endif

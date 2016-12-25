#include <ATLComTime.h>
#include <jni.h>

#include "be_derycke_pieter_com_OleDate.h"
#include "jmtp.h"

JNIEXPORT void JNICALL Java_be_derycke_pieter_com_OleDate_setDate(JNIEnv* env, jobject obj, jdouble jdValue) {
	COleDateTime date(jdValue);

	env->CallVoidMethod(obj, rt.oleDateSet,
		date.GetYear(),
		date.GetMonth() - 1,
		date.GetDay(),
		date.GetHour(),
		date.GetMinute(),
		date.GetSecond());
}

JNIEXPORT jdouble JNICALL Java_be_derycke_pieter_com_OleDate_toDouble(JNIEnv* env, jobject obj) {
	int year = env->CallIntMethod(obj, rt.oleDateGet, 0);
	int month = env->CallIntMethod(obj, rt.oleDateGet, 1);
	int day = env->CallIntMethod(obj, rt.oleDateGet, 2);
	int hour = env->CallIntMethod(obj, rt.oleDateGet, 3);
	int minute = env->CallIntMethod(obj, rt.oleDateGet, 4);
	int second = env->CallIntMethod(obj, rt.oleDateGet, 5);

	COleDateTime date(year, month, day, hour, minute, second);
	
	return DATE(date);
}

#include "org/jace/JFieldHelper.h"
#include "org/jace/Jace.h"

using org::jace::proxy::JObject;
using org::jace::JClass;
using std::string;

BEGIN_NAMESPACE_2(org, jace)

JFieldHelper::JFieldHelper(const std::string& name, const JClass& typeClass): 
  mFieldID(0), 
  mName(name), 
  mTypeClass(typeClass)
{
}


jvalue JFieldHelper::getField(JObject& object)
{
  // Get the fieldID for the field belonging to the given object.
  const JClass& parentClass = object.getJavaJniClass();
  jfieldID fieldID = getFieldID(parentClass, false);

  // Get the field value.
  JNIEnv* env = attach();
  jobject result = env->GetObjectField(static_cast<jobject>(object), fieldID);

  jvalue value;
  value.l = result;

  return value;
}


jvalue JFieldHelper::getField(const JClass& jClass)
{
  // Get the fieldID for the field belonging to the given class.
  jfieldID fieldID = getFieldID(jClass, true);

  // Get the field value.
  JNIEnv* env = attach();
  jobject result = env->GetStaticObjectField(jClass.getClass(), fieldID);

  jvalue value;
  value.l = result;

  return value;
}

jfieldID JFieldHelper::getFieldID()
{
  return mFieldID;
}

jfieldID JFieldHelper::getFieldID(const JClass& parentClass, bool isStatic)
{
  // We cache the jfieldID locally, so if we've already found it,
  // we don't need to go looking for it again.
  if (mFieldID)
    return mFieldID;

  // We could look in a global cache for the jfieldID corresponding to this field,
  // but for now, we'll always find it.
  JNIEnv* env = attach();

  string signature = mTypeClass.getNameAsType();

  if (isStatic)
    mFieldID = env->GetStaticFieldID(parentClass.getClass(), mName.c_str(), signature.c_str());
  else
    mFieldID = env->GetFieldID(parentClass.getClass(), mName.c_str(), signature.c_str());

  if (mFieldID == 0)
	{
    string msg = "JFieldHelper::getFieldID\n" \
                 "Unable to find field <" + mName + "> with signature <" + signature + ">";
		try
		{
			catchAndThrow();
		}
		catch (JNIException& e)
		{
			msg.append("\ncaused by:\n");
			msg.append(e.what());
		}
    throw JNIException(msg);
  }

//  cout << "JMethod::getMethodID() - Found the method:" << endl;
//  cout << "  <" << mName << "> with signature <" << methodSignature << "> for " << jClass.getName() << endl;

  return mFieldID;
}


END_NAMESPACE_2(org, jace)

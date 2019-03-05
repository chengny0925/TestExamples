using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;
using System.Runtime.Remoting;
using System.Text;
using System.Threading.Tasks;

namespace XXXX.Data.DataField
{
    public class TypeCreator
    {
        //public static Type Creator(string ClassName, int PropertiesCount)
        //{
        //    IDictionary<string, Type> Properties = new Dictionary<string, Type>();
        //    Type t = typeof(string);
        //    Properties.Add(new KeyValuePair<string, Type>("ID", typeof(int)));
        //    for (int i = 0; i < PropertiesCount; i++)
        //    {
        //        Properties.Add(new KeyValuePair<string, Type>("FF" + i, t));
        //    }
        //    return Creator(ClassName, Properties);
        //}
        public static Type Creator(string ClassName, IDictionary<string, Type> Properties)
        {
            //应用程序域
            AppDomain currentDomain = System.Threading.Thread.GetDomain(); //AppDomain.CurrentDomain;
            //运行并创建类的新实例
            TypeBuilder typeBuilder = null;
            //定义和表示动态程序集的模块
            ModuleBuilder moduleBuilder = null;
            MethodBuilder methodBuilder = null;
            //表示类型属性
            PropertyBuilder propertyBuilder = null;
            //定义表示字段
            FieldBuilder fieldBuilder = null;
            //定义表示动态程序集
            AssemblyBuilder assemblyBuilder = null;
            //生成中间语言指令
            ILGenerator ilGenerator = null;
            //帮助生成自定义属性
            //CustomAttributeBuilder cab = null;
            //指定方法属性的标志
            MethodAttributes methodAttrs;

            //Define a Dynamic Assembly
            //指定名称，访问模式
            assemblyBuilder = currentDomain.DefineDynamicAssembly(new AssemblyName("Test2"), AssemblyBuilderAccess.Run);

            //Define a Dynamic Module动态模块名称
            moduleBuilder = assemblyBuilder.DefineDynamicModule("ModuleName", true);

            //Define a runtime class with specified name and attributes.
            typeBuilder = moduleBuilder.DefineType(ClassName, TypeAttributes.Public | TypeAttributes.Class | TypeAttributes.BeforeFieldInit | TypeAttributes.Serializable);

            //cab = new CustomAttributeBuilder(typeof(Castle.ActiveRecord.ActiveRecordAttribute).GetConstructor(Type.EmptyTypes), new object[0]);
            //typeBuilder.SetCustomAttribute(cab);//

            //cab = new CustomAttributeBuilder(typeof(Castle.ActiveRecord.PropertyAttribute).GetConstructor(Type.EmptyTypes), new object[0]);

            methodAttrs = MethodAttributes.Public | MethodAttributes.SpecialName | MethodAttributes.HideBySig;
            foreach (KeyValuePair<string, Type> kv in Properties)
            {
                // Add the class variable, such as "m_strIPAddress"
                //fieldBuilder = typeBuilder.DefineField("field_" + kv.Key, kv.Value, FieldAttributes.Public);
                fieldBuilder = typeBuilder.DefineField(kv.Key, kv.Value, FieldAttributes.Public);

                propertyBuilder = typeBuilder.DefineProperty(kv.Key, System.Reflection.PropertyAttributes.HasDefault, kv.Value, null);
                //if (kv.Key != "ID")
                //{
                //    propertyBuilder.SetCustomAttribute(cab);//
                //}
                //else
                //{
                //    propertyBuilder.SetCustomAttribute(new CustomAttributeBuilder(typeof(Castle.ActiveRecord.PrimaryKeyAttribute).GetConstructor(Type.EmptyTypes), new object[0]));//
                //}


                //methodBuilder = typeBuilder.DefineMethod("get_" + kv.Key, methodAttrs, kv.Value, Type.EmptyTypes);
                //ilGenerator = methodBuilder.GetILGenerator();
                //ilGenerator.Emit(OpCodes.Ldarg_0);
                //ilGenerator.Emit(OpCodes.Ldfld, fieldBuilder);
                //ilGenerator.Emit(OpCodes.Ret);
                //propertyBuilder.SetGetMethod(methodBuilder);

                //methodBuilder = typeBuilder.DefineMethod("set_" + kv.Key, methodAttrs, typeof(void), new Type[] { kv.Value });
                //ilGenerator = methodBuilder.GetILGenerator();
                //ilGenerator.Emit(OpCodes.Ldarg_0);
                //ilGenerator.Emit(OpCodes.Ldarg_1);
                //ilGenerator.Emit(OpCodes.Stfld, fieldBuilder);
                //ilGenerator.Emit(OpCodes.Ret);
                //propertyBuilder.SetSetMethod(methodBuilder);
            }
            //Create Class 
            return typeBuilder.CreateType();
            return assemblyBuilder.GetType(ClassName);
            return moduleBuilder.GetType(ClassName);

        }

        public static object CreateInstance(Type type)
        {
            object ob = Activator.CreateInstance(type);
            return ob;
        }
    }
}

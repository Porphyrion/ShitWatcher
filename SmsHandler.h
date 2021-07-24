struct SmsHandler
{
    static const String addNumberCom            = "Add me";
    static const String removeMyNumberCom       = "Delete me";
    static const String removeNumberByIdCom     = "Delete id";
    static const String removeNumberByNumberCom = "Delete number";

public: 
    String resolveCommand(String& com)
    {
        if(addNumberCom.equals(com))
        {
            
        }
        else if(removeMyNumberCom.equals(com))
        {

        }
        else if(removeNumberByIdCom.equals(com))
        {

        }
        else if(removeNumberByNumberCom.equals(com))
        {

        }
    }
};
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/cpp/var_array_buffer.h"

#include <cstdint>
#include <memory>

extern "C" {
#include "webp/encode.h"

size_t WebPEncodeLosslessRGBA(const uint8_t* rgba, int width, int height, int stride, uint8_t** output);
}

class WebpencInstance : public pp::Instance {
public:
explicit WebpencInstance(PP_Instance instance) : pp::Instance(instance)
{
}
virtual ~WebpencInstance() {
}

virtual void HandleMessage(const pp::Var& var_message) {
        if (!var_message.is_dictionary ()){
            return;
        }
        pp::VarDictionary::VarDictionary  dic = pp::VarDictionary::VarDictionary (var_message);
        pp::Var s_rgba= dic.Get("rgba");
        pp::Var s_width  = dic.Get("width");
        pp::Var s_height = dic.Get("height");
        pp::Var id = dic.Get("id");

        //check
        if (!s_rgba.is_array_buffer()){
            PostMessage(ErrorMsg("ArrayBuffer no find"));
            return;
        }
        if( !s_width.is_int ()){
            PostMessage(ErrorMsg("width is not int"));
            return;
        }
        int width = s_width.AsInt ();

        if (!s_height.is_int ()){
            PostMessage(ErrorMsg("height is not int"));
            return;
        }
        int height = s_height.AsInt ();

        std::shared_ptr<pp::VarArrayBuffer::VarArrayBuffer> array_buffer_var (pp::VarArrayBuffer::VarArrayBuffer (s_rgba));

        uint8_t* data = static_cast<uint8_t *>(array_buffer_var.Map());
        int byte_length = static_cast<int>(array_buffer_var.ByteLength());

        if (byte_length != (width * height * 4)){
            PostMessage(ErrorMsg("ArrayBuffer length error :"+ std::to_string(byte_length)));
            return;
        }
        std::shared_ptr<uint8_t> outdata(new uint8_t[byte_length]());
        uint8_t * r_outdata = outdata.get();
        size_t ret = WebPEncodeLosslessRGBA(data,width,height,width*4,&r_outdata);
        if (ret == 0){
            PostMessage(ErrorMsg("encoder  error "));
            return;
        }
       pp::VarArrayBuffer::VarArrayBuffer data_value = pp::VarArrayBuffer::VarArrayBuffer (ret);
       uint8_t* r_data_value = static_cast<uint8_t*>(data_value.Map());
       for (uint32_t i = 0; i < ret; ++i){
           r_data_value[i] = r_outdata[i];
       }
       data_value.Unmap();

        pp::VarDictionary::VarDictionary  ret_msg = pp::VarDictionary::VarDictionary ();
        pp::Var id_key = pp::Var::Var("id");
        ret_msg.Set (id_key,id);
        pp::Var data_key = pp::Var::Var("data");
        ret_msg.Set (data_key,data_value);

        PostMessage(ret_msg);
}
private:
    pp::VarDictionary ErrorMsg(std::string message ){
        pp::VarDictionary::VarDictionary  error_msg = pp::VarDictionary::VarDictionary ();
        pp::Var err_key = pp::Var::Var("Error");
        pp::Var err_value = pp::Var::Var(message);
        error_msg.Set (err_key,err_value);
        return error_msg;
    }
};

class WebpencModule : public pp::Module {
public:
WebpencModule() : pp::Module() {
}
virtual ~WebpencModule() {
}

virtual pp::Instance* CreateInstance(PP_Instance instance) {
        return new WebpencInstance(instance);
}
};

namespace pp {
Module* CreateModule() {
        return new WebpencModule();
}
}  // namespace pp

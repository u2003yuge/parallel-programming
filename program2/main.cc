#include<cmath>
#include<float.h>
#include<bits/stdc++.h>
#include<arm_neon.h>
#define MSMIN(x, y) ((x) < (y) ? (x) : (y))
#define MSMAX(x, y) ((x) > (y) ? (x) : (y))
int ROIPooling_neon(int input_n,int input_h,int input_w,int input_c,int num_rois,float scale,
                int pooled_height,int pooled_width,const float *in_ptr, float *out_ptr, 
                const float *roi, float *max_c, int tid, int thread_num) {
    if (thread_num == 0) {
        return -1;        
    }
    
    int in_strides_2 = input_c;
    int in_strides_1 = in_strides_2 * input_w; 
    int in_strides_0 = in_strides_1 * input_h;
    
    int out_strides_2 = input_c;
    int out_strides_1 = pooled_height * out_strides_2;
    int out_strides_0 = pooled_width * out_strides_1;

    int units = (num_rois + thread_num - 1) / thread_num;//UP_DIV(num_rois, param->thread_num_);
    int roi_st = tid * units;
    int roi_end = MSMIN(num_rois, roi_st + units);
    if (roi_st >= num_rois) {
        return 0;
    }
    int batch_size = input_n;
    int height_ = input_h;
    int width_ = input_w;
    int channels_ = input_c;

    const int roi_stride = 5;
    int roi_ind_st = roi_st * roi_stride;
    for (int i = roi_st; i < roi_end; ++i) {
        int roi_batch_ind = (int)roi[roi_ind_st];  // batch_index
        if (roi_batch_ind >= batch_size) {
            return -1;
        }
        int roi_start_h = (int)round(roi[roi_ind_st + 1] * scale);  // top-left x1
        int roi_start_w = (int)round(roi[roi_ind_st + 2] * scale);  // top-left y1
        int roi_end_h = (int)round(roi[roi_ind_st + 3] * scale);    // bottom-right x2
        int roi_end_w = (int)round(roi[roi_ind_st + 4] * scale);    // bottom-fight y2

        int roi_height = MSMAX(roi_end_h - roi_start_h + 1, 1);
        int roi_width = MSMAX(roi_end_w - roi_start_w + 1, 1);

        float bin_size_h = (float)roi_height / (float)pooled_height;
        float bin_size_w = (float)roi_width / (float)pooled_width;
        const float *batch_data = in_ptr + in_strides_0 * roi_batch_ind;

        for (int ph = 0; ph < pooled_height; ++ph) {
            for (int pw = 0; pw < pooled_width; ++pw) {
                int hstart = (int)floor(ph * bin_size_h);     // block xi_1
                int wstart = (int)floor(pw * bin_size_w);     // block yi_1
                int hend = (int)ceil((ph + 1) * bin_size_h);  // block xi_2
                int wend = (int)ceil((pw + 1) * bin_size_w);  // block yi_2
                
                hstart = MSMIN(MSMAX(hstart + roi_start_h, 0), height_);
                hend = MSMIN(MSMAX(hend + roi_start_h, 0), height_);
                wstart = MSMIN(MSMAX(wstart + roi_start_w, 0), width_);
                wend = MSMIN(MSMAX(wend + roi_start_w, 0), width_);
                
                bool is_empty = (hend <= hstart) || (wend <= wstart);
                for (int j = 0; j < channels_; ++j) {
                    max_c[j] = is_empty ? 0 : -FLT_MAX;
                }
                int pooled_index = i * out_strides_0 + ph * out_strides_1 + pw * out_strides_2;
                int bd_index = hstart * in_strides_1;
                for (int h = hstart; h < hend; ++h) {
                    int wi = bd_index + wstart * in_strides_2;
                    for (int w = wstart; w < wend; ++w) {
                        int channels_end = channels_&(~3);
                        // std::cout<<"channels_end="<<channels_end<<std::endl;
                        for (int c = 0; c < channels_end; c+=4) {// 0 1 2 3 4 5 6 7
                            float32x4_t max_c_v = vld1q_f32(&max_c[c]);
                            float32x4_t in_v = vld1q_f32(&batch_data[wi + c]);
                            max_c_v = vmaxq_f32(max_c_v, in_v);
                            vst1q_f32(&max_c[c], max_c_v);
                            // max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                        }
                        for(int c=channels_end;c<channels_;c++){
                            max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                        }
                        wi += in_strides_2;
                    }  // in_w end;
                    bd_index += in_strides_1;
                }  // in_h end
                for (int j = 0; j < channels_; ++j) {
                    out_ptr[pooled_index + j] = max_c[j];
                }
            }
        }
        roi_ind_st += roi_stride;
    }
    return 0;
}
int ROIPooling_neon_8(int input_n,int input_h,int input_w,int input_c,int num_rois,float scale,
                int pooled_height,int pooled_width,const float *in_ptr, float *out_ptr, 
                const float *roi, float *max_c, int tid, int thread_num) {
    if (thread_num == 0) {
        return -1;        
    }
    
    int in_strides_2 = input_c;
    int in_strides_1 = in_strides_2 * input_w; 
    int in_strides_0 = in_strides_1 * input_h;
    
    int out_strides_2 = input_c;
    int out_strides_1 = pooled_height * out_strides_2;
    int out_strides_0 = pooled_width * out_strides_1;

    int units = (num_rois + thread_num - 1) / thread_num;//UP_DIV(num_rois, param->thread_num_);
    int roi_st = tid * units;
    int roi_end = MSMIN(num_rois, roi_st + units);
    if (roi_st >= num_rois) {
        return 0;
    }
    int batch_size = input_n;
    int height_ = input_h;
    int width_ = input_w;
    int channels_ = input_c;

    const int roi_stride = 5;
    int roi_ind_st = roi_st * roi_stride;
    for (int i = roi_st; i < roi_end; ++i) {
        int roi_batch_ind = (int)roi[roi_ind_st];  // batch_index
        if (roi_batch_ind >= batch_size) {
            return -1;
        }
        int roi_start_h = (int)round(roi[roi_ind_st + 1] * scale);  // top-left x1
        int roi_start_w = (int)round(roi[roi_ind_st + 2] * scale);  // top-left y1
        int roi_end_h = (int)round(roi[roi_ind_st + 3] * scale);    // bottom-right x2
        int roi_end_w = (int)round(roi[roi_ind_st + 4] * scale);    // bottom-fight y2

        int roi_height = MSMAX(roi_end_h - roi_start_h + 1, 1);
        int roi_width = MSMAX(roi_end_w - roi_start_w + 1, 1);

        float bin_size_h = (float)roi_height / (float)pooled_height;
        float bin_size_w = (float)roi_width / (float)pooled_width;
        const float *batch_data = in_ptr + in_strides_0 * roi_batch_ind;

        for (int ph = 0; ph < pooled_height; ++ph) {
            for (int pw = 0; pw < pooled_width; ++pw) {
                int hstart = (int)floor(ph * bin_size_h);     // block xi_1
                int wstart = (int)floor(pw * bin_size_w);     // block yi_1
                int hend = (int)ceil((ph + 1) * bin_size_h);  // block xi_2
                int wend = (int)ceil((pw + 1) * bin_size_w);  // block yi_2
                
                hstart = MSMIN(MSMAX(hstart + roi_start_h, 0), height_);
                hend = MSMIN(MSMAX(hend + roi_start_h, 0), height_);
                wstart = MSMIN(MSMAX(wstart + roi_start_w, 0), width_);
                wend = MSMIN(MSMAX(wend + roi_start_w, 0), width_);
                
                bool is_empty = (hend <= hstart) || (wend <= wstart);
                for (int j = 0; j < channels_; ++j) {
                    max_c[j] = is_empty ? 0 : -FLT_MAX;
                }
                int pooled_index = i * out_strides_0 + ph * out_strides_1 + pw * out_strides_2;
                int bd_index = hstart * in_strides_1;
                for (int h = hstart; h < hend; ++h) {
                    int wi = bd_index + wstart * in_strides_2;
                    for (int w = wstart; w < wend; ++w) {
                        int channels_end = channels_&(~7);
                        // std::cout<<"channels_end="<<channels_end<<std::endl;
                        for (int c = 0; c < channels_end; c+=8) {// 0 1 2 3 4 5 6 7
                            float32x4_t max_c_v1 = vld1q_f32(&max_c[c]);
                            float32x4_t max_c_v2 = vld1q_f32(&max_c[c + 4]);
                            float32x4_t in_v1 = vld1q_f32(&batch_data[wi + c]);
                            float32x4_t in_v2 = vld1q_f32(&batch_data[wi + c + 4]);
                            max_c_v1 = vmaxq_f32(max_c_v1, in_v1);
                            max_c_v2 = vmaxq_f32(max_c_v2, in_v2);
                            vst1q_f32(&max_c[c], max_c_v1);
                            vst1q_f32(&max_c[c+4], max_c_v2);
                            // max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                        }
                        for(int c=channels_end;c<channels_;c++){
                            max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                        }
                        wi += in_strides_2;
                    }  // in_w end;
                    bd_index += in_strides_1;
                }  // in_h end
                for (int j = 0; j < channels_; ++j) {
                    out_ptr[pooled_index + j] = max_c[j];
                }
            }
        }
        roi_ind_st += roi_stride;
    }
    return 0;
}
int ROIPooling_neon_16(int input_n,int input_h,int input_w,int input_c,int num_rois,float scale,
                int pooled_height,int pooled_width,const float *in_ptr, float *out_ptr, 
                const float *roi, float *max_c, int tid, int thread_num) {
    if (thread_num == 0) {
        return -1;        
    }
    
    int in_strides_2 = input_c;
    int in_strides_1 = in_strides_2 * input_w; 
    int in_strides_0 = in_strides_1 * input_h;
    
    int out_strides_2 = input_c;
    int out_strides_1 = pooled_height * out_strides_2;
    int out_strides_0 = pooled_width * out_strides_1;

    int units = (num_rois + thread_num - 1) / thread_num;//UP_DIV(num_rois, param->thread_num_);
    int roi_st = tid * units;
    int roi_end = MSMIN(num_rois, roi_st + units);
    if (roi_st >= num_rois) {
        return 0;
    }
    int batch_size = input_n;
    int height_ = input_h;
    int width_ = input_w;
    int channels_ = input_c;

    const int roi_stride = 5;
    int roi_ind_st = roi_st * roi_stride;
    for (int i = roi_st; i < roi_end; ++i) {
        int roi_batch_ind = (int)roi[roi_ind_st];  // batch_index
        if (roi_batch_ind >= batch_size) {
            return -1;
        }
        int roi_start_h = (int)round(roi[roi_ind_st + 1] * scale);  // top-left x1
        int roi_start_w = (int)round(roi[roi_ind_st + 2] * scale);  // top-left y1
        int roi_end_h = (int)round(roi[roi_ind_st + 3] * scale);    // bottom-right x2
        int roi_end_w = (int)round(roi[roi_ind_st + 4] * scale);    // bottom-fight y2

        int roi_height = MSMAX(roi_end_h - roi_start_h + 1, 1);
        int roi_width = MSMAX(roi_end_w - roi_start_w + 1, 1);

        float bin_size_h = (float)roi_height / (float)pooled_height;
        float bin_size_w = (float)roi_width / (float)pooled_width;
        const float *batch_data = in_ptr + in_strides_0 * roi_batch_ind;

        for (int ph = 0; ph < pooled_height; ++ph) {
            for (int pw = 0; pw < pooled_width; ++pw) {
                int hstart = (int)floor(ph * bin_size_h);     // block xi_1
                int wstart = (int)floor(pw * bin_size_w);     // block yi_1
                int hend = (int)ceil((ph + 1) * bin_size_h);  // block xi_2
                int wend = (int)ceil((pw + 1) * bin_size_w);  // block yi_2
                
                hstart = MSMIN(MSMAX(hstart + roi_start_h, 0), height_);
                hend = MSMIN(MSMAX(hend + roi_start_h, 0), height_);
                wstart = MSMIN(MSMAX(wstart + roi_start_w, 0), width_);
                wend = MSMIN(MSMAX(wend + roi_start_w, 0), width_);
                
                bool is_empty = (hend <= hstart) || (wend <= wstart);
                for (int j = 0; j < channels_; ++j) {
                    max_c[j] = is_empty ? 0 : -FLT_MAX;
                }
                int pooled_index = i * out_strides_0 + ph * out_strides_1 + pw * out_strides_2;
                int bd_index = hstart * in_strides_1;
                for (int h = hstart; h < hend; ++h) {
                    int wi = bd_index + wstart * in_strides_2;
                    for (int w = wstart; w < wend; ++w) {
                        int channels_end = channels_&(~15);
                        // std::cout<<"channels_end="<<channels_end<<std::endl;
                        for (int c = 0; c < channels_end; c+=16) {// 0 1 2 3 4 5 6 7
                            float32x4_t max_c_v1 = vld1q_f32(&max_c[c]);
                            float32x4_t max_c_v2 = vld1q_f32(&max_c[c + 4]);
                            float32x4_t max_c_v3 = vld1q_f32(&max_c[c + 8]);
                            float32x4_t max_c_v4 = vld1q_f32(&max_c[c + 12]);

                            float32x4_t in_v1 = vld1q_f32(&batch_data[wi + c]);
                            float32x4_t in_v2 = vld1q_f32(&batch_data[wi + c + 4]);
                            float32x4_t in_v3 = vld1q_f32(&batch_data[wi + c + 8]);
                            float32x4_t in_v4 = vld1q_f32(&batch_data[wi + c + 12]);
                            
                            max_c_v1 = vmaxq_f32(max_c_v1, in_v1);
                            max_c_v2 = vmaxq_f32(max_c_v2, in_v2);
                            max_c_v3 = vmaxq_f32(max_c_v3, in_v3);
                            max_c_v4 = vmaxq_f32(max_c_v4, in_v4);
                            
                            vst1q_f32(&max_c[c], max_c_v1);
                            vst1q_f32(&max_c[c+4], max_c_v2);
                            vst1q_f32(&max_c[c+8], max_c_v3);
                            vst1q_f32(&max_c[c+12], max_c_v4);
                            // max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                        }
                        for(int c=channels_end;c<channels_;c++){
                            max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                        }
                        wi += in_strides_2;
                    }  // in_w end;
                    bd_index += in_strides_1;
                }  // in_h end
                for (int j = 0; j < channels_; ++j) {
                    out_ptr[pooled_index + j] = max_c[j];
                }
            }
        }
        roi_ind_st += roi_stride;
    }
    return 0;
}
int ROIPooling(int input_n,int input_h,int input_w,int input_c,int num_rois,float scale,
    int pooled_height,int pooled_width,const float *in_ptr, float *out_ptr, 
    const float *roi, float *max_c, int tid, int thread_num) {
    if (thread_num == 0) {
        return -1;        
    }

    int in_strides_2 = input_c;
    int in_strides_1 = in_strides_2 * input_w; 
    int in_strides_0 = in_strides_1 * input_h;

    int out_strides_2 = input_c;
    int out_strides_1 = pooled_height * out_strides_2;
    int out_strides_0 = pooled_width * out_strides_1;

    int units = (num_rois + thread_num - 1) / thread_num;//UP_DIV(num_rois, param->thread_num_);
    int roi_st = tid * units;
    int roi_end = MSMIN(num_rois, roi_st + units);
    if (roi_st >= num_rois) {
        return 0;
    }
    int batch_size = input_n;
    int height_ = input_h;
    int width_ = input_w;
    int channels_ = input_c;

    const int roi_stride = 5;
    int roi_ind_st = roi_st * roi_stride;
    for (int i = roi_st; i < roi_end; ++i) {
        int roi_batch_ind = (int)roi[roi_ind_st];  // batch_index
        if (roi_batch_ind >= batch_size) {
            return -1;
        }
        int roi_start_h = (int)round(roi[roi_ind_st + 1] * scale);  // top-left x1
        int roi_start_w = (int)round(roi[roi_ind_st + 2] * scale);  // top-left y1
        int roi_end_h = (int)round(roi[roi_ind_st + 3] * scale);    // bottom-right x2
        int roi_end_w = (int)round(roi[roi_ind_st + 4] * scale);    // bottom-fight y2

        int roi_height = MSMAX(roi_end_h - roi_start_h + 1, 1);
        int roi_width = MSMAX(roi_end_w - roi_start_w + 1, 1);

        float bin_size_h = (float)roi_height / (float)pooled_height;
        float bin_size_w = (float)roi_width / (float)pooled_width;
        const float *batch_data = in_ptr + in_strides_0 * roi_batch_ind;

        for (int ph = 0; ph < pooled_height; ++ph) {
            for (int pw = 0; pw < pooled_width; ++pw) {
                int hstart = (int)floor(ph * bin_size_h);     // block xi_1
                int wstart = (int)floor(pw * bin_size_w);     // block yi_1
                int hend = (int)ceil((ph + 1) * bin_size_h);  // block xi_2
                int wend = (int)ceil((pw + 1) * bin_size_w);  // block yi_2
                
                hstart = MSMIN(MSMAX(hstart + roi_start_h, 0), height_);
                hend = MSMIN(MSMAX(hend + roi_start_h, 0), height_);
                wstart = MSMIN(MSMAX(wstart + roi_start_w, 0), width_);
                wend = MSMIN(MSMAX(wend + roi_start_w, 0), width_);
                
                bool is_empty = (hend <= hstart) || (wend <= wstart);
                for (int j = 0; j < channels_; ++j) {
                    max_c[j] = is_empty ? 0 : -FLT_MAX;
                }
                int pooled_index = i * out_strides_0 + ph * out_strides_1 + pw * out_strides_2;
                int bd_index = hstart * in_strides_1;
                for (int h = hstart; h < hend; ++h) {
                    int wi = bd_index + wstart * in_strides_2;
                    for (int w = wstart; w < wend; ++w) {
                        for (int c = 0; c < channels_; ++c) {
                            max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                        }
                        wi += in_strides_2;
                    }  // in_w end;
                    bd_index += in_strides_1;
                }  // in_h end
                for (int j = 0; j < channels_; ++j) {
                    out_ptr[pooled_index + j] = max_c[j];
                }
            }
        }
        roi_ind_st += roi_stride;
    }
    return 0;
}
void fRead(int *n,int *h,int *w,int *c,int *num_rois,float *scale, int *pooled_height, int *pooled_width,int input_id,float *input,float *roi){
    // �������뺯��
    std::string str1 = "files/data/";
    std::string str2 = std::to_string(input_id);
    std::string strin = str1 + str2 + ".in";
    char data_path[strin.size() + 1];
    std::copy(strin.begin(), strin.end(), data_path);
    data_path[strin.size()] = '\0';
    std::ifstream fin;
    fin.open(data_path, std::ios::in);
    std::cout<<"data_path="<<data_path<<std::endl;
    fin>>*n>>*h>>*w>>*c>>*num_rois>>*scale>>*pooled_height>>*pooled_width;
    int len=(*n)*(*h)*(*w)*(*c);
	for (int i = 0; i < len; i++){
        fin>>input[i];
    }
    for(int i=0;i<5*(*num_rois);i++){
    	fin>>roi[i];
	}
}
int ROIPooling_unroll(int input_n,int input_h,int input_w,int input_c,int num_rois,float scale,
    int pooled_height,int pooled_width,const float *in_ptr, float *out_ptr, 
    const float *roi, float *max_c, int tid, int thread_num) {
    if (thread_num == 0) {
    return -1;        
    }

    int in_strides_2 = input_c;
    int in_strides_1 = in_strides_2 * input_w; 
    int in_strides_0 = in_strides_1 * input_h;

    int out_strides_2 = input_c;
    int out_strides_1 = pooled_height * out_strides_2;
    int out_strides_0 = pooled_width * out_strides_1;

    int units = (num_rois + thread_num - 1) / thread_num;//UP_DIV(num_rois, param->thread_num_);
    int roi_st = tid * units;
    int roi_end = MSMIN(num_rois, roi_st + units);
    if (roi_st >= num_rois) {
    return 0;
    }
    int batch_size = input_n;
    int height_ = input_h;
    int width_ = input_w;
    int channels_ = input_c;

    const int roi_stride = 5;
    int roi_ind_st = roi_st * roi_stride;
    for (int i = roi_st; i < roi_end; ++i) {
    int roi_batch_ind = (int)roi[roi_ind_st];  // batch_index
    if (roi_batch_ind >= batch_size) {
    return -1;
    }
    int roi_start_h = (int)round(roi[roi_ind_st + 1] * scale);  // top-left x1
    int roi_start_w = (int)round(roi[roi_ind_st + 2] * scale);  // top-left y1
    int roi_end_h = (int)round(roi[roi_ind_st + 3] * scale);    // bottom-right x2
    int roi_end_w = (int)round(roi[roi_ind_st + 4] * scale);    // bottom-fight y2

    int roi_height = MSMAX(roi_end_h - roi_start_h + 1, 1);
    int roi_width = MSMAX(roi_end_w - roi_start_w + 1, 1);

    float bin_size_h = (float)roi_height / (float)pooled_height;
    float bin_size_w = (float)roi_width / (float)pooled_width;
    const float *batch_data = in_ptr + in_strides_0 * roi_batch_ind;

    for (int ph = 0; ph < pooled_height; ++ph) {
    for (int pw = 0; pw < pooled_width; ++pw) {
        int hstart = (int)floor(ph * bin_size_h);     // block xi_1
        int wstart = (int)floor(pw * bin_size_w);     // block yi_1
        int hend = (int)ceil((ph + 1) * bin_size_h);  // block xi_2
        int wend = (int)ceil((pw + 1) * bin_size_w);  // block yi_2
        
        hstart = MSMIN(MSMAX(hstart + roi_start_h, 0), height_);
        hend = MSMIN(MSMAX(hend + roi_start_h, 0), height_);
        wstart = MSMIN(MSMAX(wstart + roi_start_w, 0), width_);
        wend = MSMIN(MSMAX(wend + roi_start_w, 0), width_);
        
        bool is_empty = (hend <= hstart) || (wend <= wstart);
        for (int j = 0; j < channels_; ++j) {
            max_c[j] = is_empty ? 0 : -FLT_MAX;
        }
        int pooled_index = i * out_strides_0 + ph * out_strides_1 + pw * out_strides_2;
        int bd_index = hstart * in_strides_1;
        for (int h = hstart; h < hend; ++h) {
            int wi = bd_index + wstart * in_strides_2;
            for (int w = wstart; w < wend; ++w) {
                int channels_end = channels_&(~3);
                // std::cout<<"channels_end="<<channels_end<<std::endl;
                for (int c = 0; c < channels_end; c+=4) {// 0 1 2 3 4 5 6 7
                    max_c[c]=MSMAX(batch_data[wi + c], max_c[c]);
                    max_c[c+1]=MSMAX(batch_data[wi + c+1], max_c[c+1]);
                    max_c[c+2]=MSMAX(batch_data[wi + c+2], max_c[c+2]);
                    max_c[c+3]=MSMAX(batch_data[wi + c+3], max_c[c+3]);
                    // vst1q_f32(&max_c[c], max_c_v);
                    // max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                }
                for(int c=channels_end;c<channels_;c++){
                    max_c[c] = MSMAX(batch_data[wi + c], max_c[c]);
                }
                wi += in_strides_2;
            }  // in_w end;
            bd_index += in_strides_1;
        }  // in_h end
        for (int j = 0; j < channels_; ++j) {
            out_ptr[pooled_index + j] = max_c[j];
        }
    }
    }
    roi_ind_st += roi_stride;
    }
    return 0;
}
void fCheck(float *ab, int len, int input_id){
    // �ж϶���ʽ�˷�����Ƿ���ȷ
    std::string str1 = "files/data/";
    std::string str2 = std::to_string(input_id);
    std::string strout = str1 + str2 + ".out";
    char data_path[strout.size() + 1];
    std::copy(strout.begin(), strout.end(), data_path);
    data_path[strout.size()] = '\0';
    std::ifstream fin;
    fin.open(data_path, std::ios::in);
    std::cout<<"data_path="<<data_path<<std::endl;
    for (int i = 0; i < len; i++){
        float x;
        fin>>x;
        if(x != ab[i]){
            std::cout<<"expected: "<<x<<std::endl;
            std::cout<<"factual: "<<ab[i]<<std::endl;
            std::cout<<"ohno"<<std::endl;
            return;
        }
    }
    std::cout<<"success"<<std::endl;
    return;
}

void fWrite(const float *out, int len, int input_id){
    // �����������, ��������������ս��, Ҳ�����ڵ���ʱ����м�����
    std::string str1 = "files/";
    std::string str2 = std::to_string(input_id);
    std::string strout = str1 + str2 + ".out";
    char output_path[strout.size() + 1];
    std::copy(strout.begin(), strout.end(), output_path);
    output_path[strout.size()] = '\0';
    std::ofstream fout;
    fout.open(output_path, std::ios::out);
    for (int i = 0; i < len; i++){
        fout<<out[i]<<' ';
        // std::cout<<out[i]<<' ';
    }
    fout.close();
}
float in[30000000],out[30000000],max_c[300000],roi[300000]; 
void test_neon_c(){
	int n,h,w,c,num_rois;
	float scale;
	int pooled_height,pooled_width;
    for(int input_id=10;input_id<16;input_id++){
        std::cout<<"input_id: "<<input_id<<std::endl;
        fRead(&n,&h,&w,&c,&num_rois,&scale,&pooled_height,&pooled_width,input_id,in,roi);
        std::cout<<"n="<<n<<" h="<<h<<" w="<<w<<" c="<<c<<" num_rois="<<num_rois<<" scale="<<scale<<" pooled_height="<<pooled_height<<" pooled_width="<<pooled_width<<std::endl;
        
        auto Start1 = std::chrono::high_resolution_clock::now();
        ROIPooling(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed1 = End1 - Start1;
        std::cout<<"latency for ori "<<input_id<<" t = "<<elapsed1.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);

        auto Start2 = std::chrono::high_resolution_clock::now();
        ROIPooling_neon(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed2 = End2 - Start2;
        std::cout<<"latency for neon "<<input_id<<" t = "<<elapsed2.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);
    }
}
void test_unroll_c(){
	int n,h,w,c,num_rois;
	float scale;
	int pooled_height,pooled_width;
    for(int input_id=10;input_id<16;input_id++){
        std::cout<<"input_id: "<<input_id<<std::endl;
        fRead(&n,&h,&w,&c,&num_rois,&scale,&pooled_height,&pooled_width,input_id,in,roi);
        std::cout<<"n="<<n<<" h="<<h<<" w="<<w<<" c="<<c<<" num_rois="<<num_rois<<" scale="<<scale<<" pooled_height="<<pooled_height<<" pooled_width="<<pooled_width<<std::endl;
        

        auto Start1 = std::chrono::high_resolution_clock::now();
        ROIPooling_unroll(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed1 = End1 - Start1;
        std::cout<<"latency for unroll "<<input_id<<" t = "<<elapsed1.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);

        auto Start2 = std::chrono::high_resolution_clock::now();
        ROIPooling_neon(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed2 = End2 - Start2;
        std::cout<<"latency for neon "<<input_id<<" t = "<<elapsed2.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);
    }
}
void test_all_c(){
	int n,h,w,c,num_rois;
	float scale;
	int pooled_height,pooled_width;
    for(int input_id=10;input_id<16;input_id++){
        std::cout<<"input_id: "<<input_id<<std::endl;
        fRead(&n,&h,&w,&c,&num_rois,&scale,&pooled_height,&pooled_width,input_id,in,roi);
        std::cout<<"n="<<n<<" h="<<h<<" w="<<w<<" c="<<c<<" num_rois="<<num_rois<<" scale="<<scale<<" pooled_height="<<pooled_height<<" pooled_width="<<pooled_width<<std::endl;
        
        auto Start0 = std::chrono::high_resolution_clock::now();
        ROIPooling(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End0 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed0 = End0 - Start0;
        std::cout<<"latency for ori "<<input_id<<" t = "<<elapsed0.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);

        auto Start1 = std::chrono::high_resolution_clock::now();
        ROIPooling_unroll(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed1 = End1 - Start1;
        std::cout<<"latency for unroll "<<input_id<<" t = "<<elapsed1.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);

        auto Start2 = std::chrono::high_resolution_clock::now();
        ROIPooling_neon(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed2 = End2 - Start2;
        std::cout<<"latency for neon "<<input_id<<" t = "<<elapsed2.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);
    }
}
void test_neon_multi_2n(){
	int n,h,w,c,num_rois;
	float scale;
	int pooled_height,pooled_width;
    for(int input_id=12;input_id<16;input_id++){

        std::cout<<"input_id: "<<input_id<<std::endl;
        fRead(&n,&h,&w,&c,&num_rois,&scale,&pooled_height,&pooled_width,input_id,in,roi);
        std::cout<<"n="<<n<<" h="<<h<<" w="<<w<<" c="<<c<<" num_rois="<<num_rois<<" scale="<<scale<<" pooled_height="<<pooled_height<<" pooled_width="<<pooled_width<<std::endl;
        
        auto Start0 = std::chrono::high_resolution_clock::now();
        ROIPooling_neon(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End0 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed0 = End0 - Start0;
        std::cout<<"latency for neon4 "<<input_id<<" t = "<<elapsed0.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);

        auto Start1 = std::chrono::high_resolution_clock::now();
        ROIPooling_neon_8(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed1 = End1 - Start1;
        std::cout<<"latency for neon8 "<<input_id<<" t = "<<elapsed1.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);

        auto Start2 = std::chrono::high_resolution_clock::now();
        ROIPooling_neon_16(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
        auto End2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double,std::ratio<1,1000>>elapsed2 = End2 - Start2;
        std::cout<<"latency for neon16 "<<input_id<<" t = "<<elapsed2.count()<<" ms"<<std::endl;
        fCheck(out,num_rois*c*pooled_height*pooled_width,input_id);
    }
}

void test_neon_only(){
	int n,h,w,c,num_rois;
	float scale;
	int pooled_height,pooled_width;
    int input_id=16;
    fRead(&n,&h,&w,&c,&num_rois,&scale,&pooled_height,&pooled_width,input_id,in,roi);
    ROIPooling_neon(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
}
void test_naive_only(){
	int n,h,w,c,num_rois;
	float scale;
	int pooled_height,pooled_width;
    int input_id=16;
    fRead(&n,&h,&w,&c,&num_rois,&scale,&pooled_height,&pooled_width,input_id,in,roi);
    ROIPooling(n,h,w,c,num_rois,scale,pooled_height,pooled_width,in, out, roi, max_c, 0, 1);
}
signed main(int argc, char *argv[]){
	int input_id=argc>1?atoi(argv[1]):4;
    // test_neon_c();
    // test_all_c();
    test_neon_only();
    // test_neon_multi_2n();
	// fWrite(out,num_rois*c*pooled_height*pooled_width,input_id);
	return 0;
} 

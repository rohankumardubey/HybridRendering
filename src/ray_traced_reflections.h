#pragma once

#include <vk.h>

struct CommonResources;
class GBuffer;

class RayTracedReflections
{
public:
    enum Output
    {
        OUTPUT_RAY_TRACE,
        OUTPUT_TEMPORAL_ACCUMULATION,
        OUTPUT_ATROUS
    };

    const static int         kNumOutputTypes = 3;
    const static Output      kOutputTypeEnums[];
    const static std::string kOutputTypeNames[];

public:
    RayTracedReflections(std::weak_ptr<dw::vk::Backend> backend, CommonResources* common_resources, GBuffer* g_buffer, uint32_t width, uint32_t height);
    ~RayTracedReflections();

    void                       render(dw::vk::CommandBuffer::Ptr cmd_buf);
    void                       gui();
    dw::vk::DescriptorSet::Ptr output_ds(Output output_type = OUTPUT_ATROUS);

    inline uint32_t width() { return m_width; }
    inline uint32_t height() { return m_height; }

private:
    void create_images();
    void create_descriptor_sets();
    void write_descriptor_sets();
    void create_pipelines();
    void clear_images(dw::vk::CommandBuffer::Ptr cmd_buf);
    void ray_trace(dw::vk::CommandBuffer::Ptr cmd_buf);
    void temporal_accumulation(dw::vk::CommandBuffer::Ptr cmd_buf);
    void a_trous_filter(dw::vk::CommandBuffer::Ptr cmd_buf);

private:
    struct RayTrace
    {
        float                           bias = 0.5f;
        float                           trim = 0.8f;
        dw::vk::DescriptorSet::Ptr      write_ds;
        dw::vk::DescriptorSet::Ptr      read_ds;
        dw::vk::RayTracingPipeline::Ptr pipeline;
        dw::vk::PipelineLayout::Ptr     pipeline_layout;
        dw::vk::Image::Ptr              image;
        dw::vk::ImageView::Ptr          view;
        dw::vk::ShaderBindingTable::Ptr sbt;
    };

    struct TemporalAccumulation
    {
        float                            alpha         = 0.01f;
        float                            moments_alpha = 0.2f;
        dw::vk::ComputePipeline::Ptr     pipeline;
        dw::vk::PipelineLayout::Ptr      pipeline_layout;
        dw::vk::DescriptorSetLayout::Ptr write_ds_layout;
        dw::vk::DescriptorSetLayout::Ptr read_ds_layout;
        dw::vk::Image::Ptr               current_output_image;
        dw::vk::Image::Ptr               current_moments_image[2];
        dw::vk::Image::Ptr               prev_image;
        dw::vk::ImageView::Ptr           current_output_view;
        dw::vk::ImageView::Ptr           current_moments_view[2];
        dw::vk::ImageView::Ptr           prev_view;
        dw::vk::DescriptorSet::Ptr       current_write_ds[2];
        dw::vk::DescriptorSet::Ptr       current_read_ds[2];
        dw::vk::DescriptorSet::Ptr       output_only_read_ds;
        dw::vk::DescriptorSet::Ptr       prev_read_ds[2];
    };

    struct ATrous
    {
        float                        phi_color     = 10.0f;
        float                        phi_normal         = 32.0f;
        int32_t                      radius             = 1;
        int32_t                      filter_iterations  = 4;
        int32_t                      feedback_iteration = 1;
        int32_t                      read_idx           = 0;
        dw::vk::ComputePipeline::Ptr pipeline;
        dw::vk::PipelineLayout::Ptr  pipeline_layout;
        dw::vk::Image::Ptr           image[2];
        dw::vk::ImageView::Ptr       view[2];
        dw::vk::DescriptorSet::Ptr   read_ds[2];
        dw::vk::DescriptorSet::Ptr   write_ds[2];
    };

    std::weak_ptr<dw::vk::Backend> m_backend;
    CommonResources*               m_common_resources;
    GBuffer*                       m_g_buffer;
    uint32_t                       m_g_buffer_mip = 0;
    uint32_t                       m_width;
    uint32_t                       m_height;
    bool                           m_denoise = true;
    RayTrace                       m_ray_trace;
    TemporalAccumulation           m_temporal_accumulation;
    ATrous                         m_a_trous;
};
use crate::vertex::Vertex;

pub struct RenderPipelineFactory {
      vs_module: wgpu::ShaderModule,
      pub uniform_bind_group_layout: wgpu::BindGroupLayout,
      render_pipeline_layout: wgpu::PipelineLayout,
  }
  
  impl RenderPipelineFactory {
      pub fn new(device: &wgpu::Device) -> Self {
          let vs_module = device.create_shader_module(&wgpu::include_spirv!("shaders/shader.vert.spv"));
          let uniform_bind_group_layout = device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
              entries: &[
                  wgpu::BindGroupLayoutEntry {
                      binding: 0,
                      visibility: wgpu::ShaderStage::VERTEX,
                      ty: wgpu::BindingType::Buffer {
                          ty: wgpu::BufferBindingType::Uniform,
                          has_dynamic_offset: false,
                          min_binding_size: None,
                      },
                      count: None,
                  }
              ],
              label: Some("uniform_bind_group_layout"),
          });        
          let render_pipeline_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
              label: Some("Render Pipeline Layout"),
              bind_group_layouts: &[
                  &uniform_bind_group_layout,
              ],
              push_constant_ranges: &[],
          });
          Self {
              vs_module,
              uniform_bind_group_layout,
              render_pipeline_layout,
          }
      }
  
      pub fn create(&self, device: &wgpu::Device, fs_module: wgpu::ShaderModule, format: wgpu::TextureFormat) -> wgpu::RenderPipeline {
          device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
              label: Some("Render Pipeline"),
              layout: Some(&self.render_pipeline_layout),
              vertex: wgpu::VertexState {
                  module: &self.vs_module,
                  entry_point: "main",
                  buffers: &[
                      Vertex::desc(),
                  ],
              },
              fragment: Some(wgpu::FragmentState {
                  module: &fs_module,
                  entry_point: "main",
                  targets: &[wgpu::ColorTargetState {
                      format: format,
                      alpha_blend: wgpu::BlendState::REPLACE,
                      color_blend: wgpu::BlendState::REPLACE,
                      write_mask: wgpu::ColorWrite::ALL,
                  }],
              }),
              primitive: wgpu::PrimitiveState {
                  topology: wgpu::PrimitiveTopology::TriangleList,
                  strip_index_format: None,
                  front_face: wgpu::FrontFace::Ccw,
                  cull_mode: wgpu::CullMode::Back,
                  // Setting this to anything other than Fill requires Features::NON_FILL_POLYGON_MODE
                  polygon_mode: wgpu::PolygonMode::Fill,
              },
              depth_stencil: None,
              multisample: wgpu::MultisampleState {
                  count: 1,
                  mask: !0,
                  alpha_to_coverage_enabled: false,
              },
          })
      }
  }
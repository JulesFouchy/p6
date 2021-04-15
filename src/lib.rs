use std::iter;

use wgpu::util::DeviceExt;
mod texture;
mod render_pipeline_factory;
use render_pipeline_factory::RenderPipelineFactory;
mod vertex;
use vertex::Vertex;
use rand::Rng;

use winit::{
    event::*,
    event_loop::{ControlFlow, EventLoop},
    window::{Window, WindowBuilder},
};

const VERTICES: &[Vertex] = &[
    Vertex { position: [-1., -1.], tex_coords: [0., 1.], },
    Vertex { position: [ 1., -1.], tex_coords: [1., 1.], },
    Vertex { position: [ 1.,  1.], tex_coords: [1., 0.], },
    Vertex { position: [-1.,  1.], tex_coords: [0., 0.], },
];

const INDICES: &[u16] = &[
    0, 1, 2,
    0, 2, 3,
    0, 1, 3,
];

#[repr(C)]
#[derive(Debug, Copy, Clone, bytemuck::Pod, bytemuck::Zeroable)]
struct Uniforms {
    model_matrix: [[f32; 4]; 4],
    aspect_ratio: f32,
}

impl Uniforms {
    fn new(aspect_ratio: f32) -> Self {
        use cgmath::SquareMatrix;
        Self {
            model_matrix: cgmath::Matrix4::identity().into(),
            aspect_ratio,
        }
    }

    fn update_model_matrix(&mut self, x: f32, y: f32, w: f32, h: f32, rotation: f32) {
        self.model_matrix = 
        (
            cgmath::Matrix4::from_translation(cgmath::Vector3{x, y, z: 0.}) *
            cgmath::Matrix4::from_angle_z(cgmath::Rad(rotation)) *
            cgmath::Matrix4::from_nonuniform_scale(w, h, 1.)
        ).into();
    }
}

pub struct State {
    surface: wgpu::Surface,
    device: wgpu::Device,
    queue: wgpu::Queue,
    sc_desc: wgpu::SwapChainDescriptor,
    swap_chain: wgpu::SwapChain,
    size: winit::dpi::PhysicalSize<u32>,
    rect_render_pipeline: wgpu::RenderPipeline,
    ellipse_render_pipeline: wgpu::RenderPipeline,
    vertex_buffer: wgpu::Buffer,
    index_buffer: wgpu::Buffer,
    uniforms: Uniforms,
    uniform_buffer: wgpu::Buffer,
    uniform_bind_group: wgpu::BindGroup,
    swap_chain_texture: Option<wgpu::SwapChainTexture>,
}

impl State {
    async fn new(window: &Window) -> Self {
        let size = window.inner_size();
        let aspect_ratio = size.width as f32 / size.height as f32;

        // The instance is a handle to our GPU
        // BackendBit::PRIMARY => Vulkan + Metal + DX12 + Browser WebGPU
        let instance = wgpu::Instance::new(wgpu::BackendBit::PRIMARY);
        let surface = unsafe { instance.create_surface(window) };
        let adapter = instance
            .request_adapter(&wgpu::RequestAdapterOptions {
                power_preference: wgpu::PowerPreference::default(),
                compatible_surface: Some(&surface),
            })
            .await
            .unwrap();

        let (device, queue) = adapter
            .request_device(
                &wgpu::DeviceDescriptor {
                    label: None,
                    features: wgpu::Features::empty(),
                    limits: wgpu::Limits::default(),
                },
                None, // Trace path
            )
            .await
            .unwrap();

        let sc_desc = wgpu::SwapChainDescriptor {
            usage: wgpu::TextureUsage::RENDER_ATTACHMENT,
            format: wgpu::TextureFormat::Bgra8UnormSrgb,
            width: size.width,
            height: size.height,
            present_mode: wgpu::PresentMode::Fifo,
        };
        let swap_chain = device.create_swap_chain(&surface, &sc_desc);

        let uniforms = Uniforms::new(aspect_ratio);

        let uniform_buffer = device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Uniform Buffer"),
                contents: bytemuck::cast_slice(&[uniforms]),
                usage: wgpu::BufferUsage::UNIFORM | wgpu::BufferUsage::COPY_DST,
            }
        );

        let render_pipeline_factory = RenderPipelineFactory::new(&device);
        
        let uniform_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &render_pipeline_factory.uniform_bind_group_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: uniform_buffer.as_entire_binding(),
                }
            ],
            label: Some("uniform_bind_group"),
        });



        let rect_fs_module = device.create_shader_module(&wgpu::include_spirv!("shaders/rect.frag.spv"));
        let rect_render_pipeline = render_pipeline_factory.create(&device, rect_fs_module, sc_desc.format);
        let ellipse_fs_module = device.create_shader_module(&wgpu::include_spirv!("shaders/ellipse.frag.spv"));
        let ellipse_render_pipeline = render_pipeline_factory.create(&device, ellipse_fs_module, sc_desc.format);

        let vertex_buffer = device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Vertex Buffer"),
                contents: bytemuck::cast_slice(VERTICES),
                usage: wgpu::BufferUsage::VERTEX,
            }
        );
        
        let index_buffer = device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Index Buffer"),
                contents: bytemuck::cast_slice(INDICES),
                usage: wgpu::BufferUsage::INDEX,
            }
        );

        Self {
            surface,
            device,
            queue,
            size,
            sc_desc,
            swap_chain,
            rect_render_pipeline,
            ellipse_render_pipeline,
            vertex_buffer,
            index_buffer,
            uniforms,
            uniform_buffer,
            uniform_bind_group,
            swap_chain_texture: None,
        }
    }

    fn resize(&mut self, new_size: winit::dpi::PhysicalSize<u32>) {
        self.size = new_size;
        self.uniforms.aspect_ratio = new_size.width as f32 / new_size.height as f32;
        self.sc_desc.width = new_size.width;
        self.sc_desc.height = new_size.height;
        self.swap_chain = self.device.create_swap_chain(&self.surface, &self.sc_desc);
    }

    fn input(&mut self, event: &WindowEvent) -> bool {
        return false;
    }

    fn update(&mut self) {
        self.queue.write_buffer(&self.uniform_buffer, 0, bytemuck::cast_slice(&[self.uniforms]));
    }

    fn begin_rendering(&mut self) -> Result<(), wgpu::SwapChainError> {
        match self.swap_chain.get_current_frame() {
            Ok(frame) => {
                self.swap_chain_texture = Some(frame.output);
                Ok(())
            },
            Err(e) => {
                self.swap_chain_texture = None;
                Err(e)
            },
        }
    }

    fn end_rendering(&mut self) {
        self.swap_chain_texture = None;
    }

    fn render(&mut self) {
        let mut rng = rand::thread_rng();
        self.background();
        self.rect(0.5, 0., 0.75, 0.75, 0.5 * std::f32::consts::TAU);
        self.ellipse(rng.gen_range(-1.0..1.0), 0., 0.25, 0.25, 0.25 * std::f32::consts::TAU);
    }

    fn background(&mut self) {
        match &self.swap_chain_texture {
            Some(tex) => {
                let mut encoder1 = self
                    .device
                    .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                        label: Some("Render Encoder"),
                    });

                // self.uniforms.update_model_matrix(0.);
                self.queue.write_buffer(&self.uniform_buffer, 0, bytemuck::cast_slice(&[self.uniforms]));

                {
                    let mut render_pass = encoder1.begin_render_pass(&wgpu::RenderPassDescriptor {
                        label: Some("Render Pass"),
                        color_attachments: &[wgpu::RenderPassColorAttachmentDescriptor {
                            attachment: &tex.view,
                            resolve_target: None,
                            ops: wgpu::Operations {
                                load: wgpu::LoadOp::Clear(wgpu::Color {
                                    r: 0.1,
                                    g: 0.2,
                                    b: 0.3,
                                    a: 1.0,
                                }),
                                store: true,
                            },
                        }],
                        depth_stencil_attachment: None,
                    });

                    render_pass.set_pipeline(&self.rect_render_pipeline);
                    render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
                    render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
                    render_pass.set_index_buffer(self.index_buffer.slice(..), wgpu::IndexFormat::Uint16);
                    render_pass.draw_indexed(0..0, 0, 0..1);
                }

                self.queue.submit(iter::once(encoder1.finish()));
            },
            None => {

            },
        }
    }

    fn rect_with_shader(&self, render_pipeline: &wgpu::RenderPipeline) {
        match &self.swap_chain_texture {
            Some(tex) => {
                let mut encoder2 = self
                .device
                .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                    label: Some("Render Encoder"),
                });

                self.queue.write_buffer(&self.uniform_buffer, 0, bytemuck::cast_slice(&[self.uniforms]));

                {
                    let mut render_pass = encoder2.begin_render_pass(&wgpu::RenderPassDescriptor {
                        label: Some("Render Pass"),
                        color_attachments: &[wgpu::RenderPassColorAttachmentDescriptor {
                            attachment: &tex.view,
                            resolve_target: None,
                            ops: wgpu::Operations {
                                load: wgpu::LoadOp::Load,
                                store: true,
                            },
                        }],
                        depth_stencil_attachment: None,
                    });

                    render_pass.set_pipeline(&render_pipeline);
                    render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
                    render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
                    render_pass.set_index_buffer(self.index_buffer.slice(..), wgpu::IndexFormat::Uint16);
                    render_pass.draw_indexed(0..6, 0, 0..1);
                }
                
                self.queue.submit(iter::once(encoder2.finish()));
            },
            None => {

            },
        }
    }

    fn rect(&mut self, x: f32, y: f32, w: f32, h: f32, rotation: f32) {
        self.uniforms.update_model_matrix(x, y, w, h, rotation);
        self.rect_with_shader(&self.rect_render_pipeline);
    }

    fn ellipse(&mut self, x: f32, y: f32, w: f32, h: f32, rotation: f32) {
        self.uniforms.update_model_matrix(x, y, w, h, rotation);
        self.rect_with_shader(&self.ellipse_render_pipeline);
    }

    pub fn run() {

        env_logger::init();
        let event_loop = EventLoop::new();
        let window = WindowBuilder::new().build(&event_loop).unwrap();
    
        use futures::executor::block_on;
    
        // Since main can't be async, we're going to need to block
        let mut state = block_on(State::new(&window));
    
        event_loop.run(move |event, _, control_flow| {
            match event {
                Event::WindowEvent {
                    ref event,
                    window_id,
                } if window_id == window.id() => {
                    if !state.input(event) {
                        match event {
                            WindowEvent::CloseRequested => *control_flow = ControlFlow::Exit,
                            WindowEvent::KeyboardInput { input, .. } => match input {
                                KeyboardInput {
                                    state: ElementState::Pressed,
                                    virtual_keycode: Some(VirtualKeyCode::Escape),
                                    ..
                                } => *control_flow = ControlFlow::Exit,
                                _ => {}
                            },
                            WindowEvent::Resized(physical_size) => {
                                state.resize(*physical_size);
                            }
                            WindowEvent::ScaleFactorChanged { new_inner_size, .. } => {
                                // new_inner_size is &mut so w have to dereference it twice
                                state.resize(**new_inner_size);
                            }
                            _ => {}
                        }
                    }
                }
                Event::RedrawRequested(_) => {
                    state.update();
                    match state.begin_rendering() {
                        Ok(_) => {}
                        // Recreate the swap_chain if lost
                        Err(wgpu::SwapChainError::Lost) => state.resize(state.size),
                        // The system is out of memory, we should probably quit
                        Err(wgpu::SwapChainError::OutOfMemory) => *control_flow = ControlFlow::Exit,
                        // All other errors (Outdated, Timeout) should be resolved by the next frame
                        Err(e) => eprintln!("{:?}", e),
                    }
                    state.render();
                    state.end_rendering();
                }
                Event::MainEventsCleared => {
                    // RedrawRequested will only trigger once, unless we manually
                    // request it.
                    window.request_redraw();
                }
                _ => {}
            }
        });
    }
}
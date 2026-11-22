
#include "VulkanVisualizer.h"


int main()
{
    VulkanVisualizer visualizer;

    bool run3DDemo = true;

    if (run3DDemo == true)
    {
        visualizer.SetInitialVisualizerMode(false);
        visualizer.SetInitialRendererName("VulkanVisualizer");
        visualizer.SetInitialWindowMode(VulkanWindowMode::Window, 1280, 720);
        visualizer.SetInitialMaxVertexCount(100000);
        visualizer.SetInitialUseMSAA(true, 8);
        visualizer.SetInitialShowLiveTimingInfo(false);
        visualizer.SetInitialShowLiveFrameInfo(false);
        visualizer.SetInitialShowPostCloseTimingInfo(true);


        bool cursorIsCaptured = true;

        std::array<float, 3> lightSourceColor = {1.0f, 1.0f, 1.0f};
        std::array<float, 3> lightSourcePosition = {-16.0f, -16.0f, 30.0f};

        std::array<float, 3> cameraPosition = {10.0f, -30.0f, 30.0f};
        std::array<float, 3> cameraTargetPosition = {10.0f, 0.0f, 0.0f};
        float cameraSpeed = 10.0f;
        float cameraSensitivity = 0.001f;

        try
        {
            visualizer.Init();

            visualizer.SetCursorIsCaptured(cursorIsCaptured);

            visualizer.SetLightSourceColor(lightSourceColor[0], lightSourceColor[1], lightSourceColor[2]);
            visualizer.SetLightSourcePosition(lightSourcePosition[0], lightSourcePosition[1], lightSourcePosition[2]);

            visualizer.SetCameraPosition(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
            visualizer.SetCameraTargetPosition(cameraTargetPosition[0], cameraTargetPosition[1], cameraTargetPosition[2]);

            while (visualizer.WindowShouldClose() == false)
            {
                visualizer.HandleFrameStart();

                if (visualizer.IsKeyJustPressed(GLFW_KEY_TAB) == true)
                {
                    cursorIsCaptured = !cursorIsCaptured;
                    visualizer.SetCursorIsCaptured(cursorIsCaptured);
                }

                if (visualizer.IsKeyJustPressed(GLFW_KEY_F1) == true)
                {
                    visualizer.SetWindowMode(VulkanWindowMode::Window);
                }
                else if (visualizer.IsKeyJustPressed(GLFW_KEY_F2) == true)
                {
                    visualizer.SetWindowMode(VulkanWindowMode::BorderlessFullscreen);
                }
                else if (visualizer.IsKeyJustPressed(GLFW_KEY_F3) == true)
                {
                    visualizer.SetWindowMode(VulkanWindowMode::ExclusiveFullscreen);
                }

                visualizer.Draw3DSphere<2>(lightSourcePosition[0], lightSourcePosition[1], lightSourcePosition[2], 1.0f, 1.0f, 0.0f, 0.9f, 1.0f);

                // visualizer.SetLightSourcePosition(15.0f + 40.0f*std::sinf(visualizer.GetSimulationTime<float>()/5.0f), 10.0f + 40.0f*std::cosf(visualizer.GetSimulationTime<float>()/5.0f), lightSourcePosition[2]);
                // visualizer.Draw3DSphere<2>(15.0f + 40.0f*std::sinf(visualizer.GetSimulationTime<float>()/5.0f), 10.0f + 40.0f*std::cosf(visualizer.GetSimulationTime<float>()/5.0f), lightSourcePosition[2], 1.0f, 1.0f, 0.0, 0.9f, 1.0f);


                visualizer.Draw3DTriangle(1.0f, 1.0f, 1.0f, 8.0f, 7.0f, 4.0f, 2.0f, 8.0f, 2.0f, 1.0f, 0.0f, 0.0, 1.0f);
                visualizer.Draw3DTriangle(4.0f, 11.0f, 1.0f, 8.0f, 17.0f, 2.0f, 2.0f, 18.0f, 4.0f, 0.0f, 1.0f, 0.0, 0.1f);

                visualizer.Draw3DIcosahedron(15.0f, 5.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 7.0f);
                visualizer.Draw3DIcosahedron(15.0f, 15.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 9.0f);
                visualizer.Draw3DIcosahedron(15.0f, 15.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 6.0f);

                visualizer.Draw3DRectangularCuboid(23.5f, 3.5f, -1.5f, 26.5f, 6.5f, 1.5f, 0.0f, 0.0f, 1.0f, 1.0f);
                visualizer.Draw3DCube(25.0f, 15.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 7.0f);
                visualizer.Draw3DCube(25.0f, 15.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 4.0f);

                visualizer.Draw3DSphere<1>(35.0f, 1.0f + 4.5f/2.0f, 0.0f, 0.549f, 0.030f, 0.628f, 1.0f, 4.5f);
                visualizer.Draw3DSphere<2>(35.0f, 1.0f + 4.5f + 1.0f + 6.0f/2.0f, 0.0f, 0.176f, 0.688f, 0.774f, 1.0f, 6.0f);
                visualizer.Draw3DSphere<3>(35.0f, 1.0f + 4.5f + 1.0f + 6.0f + 1.0f + 7.5f/2.0f, 0.0f, 0.913f, 0.786f, 0.452f, 1.0f, 7.5f);
                visualizer.Draw3DSphere<4>(35.0f, 1.0f + 4.5f + 1.0f + 6.0f + 1.0f + 7.5f + 1.0f + 9.0f/2.0f, 0.0f, 0.917f, 0.480f, 0.202f, 1.0f, 9.0f);

                visualizer.Draw3DLineSegment<3>(1.0f, -3.0f, 1.0f, 19.0f, -3.0f, 1.0f, 0.059f, 0.685f, 0.294f, 1.0f, 2.0f);
                visualizer.Draw3DLineSegment<5>(1.0f, -6.0f, 1.0f, 19.0f, -6.0f, 4.0f, 0.475f, 0.826f, 0.882f, 1.0f, 2.0f, 0.0f, 0.5f);
                visualizer.Draw3DLineSegment<7>(1.0f, -9.0f, 1.0f, 19.0f, -9.0f, 7.0f, 0.174f, 0.653f, 0.885f, 1.0f, 2.0f, 0.0f, 1.0f);
                visualizer.Draw3DLineSegment<9>(1.0f, -12.0f, 1.0f, 19.0f, -12.0f, 10.0f, 0.350f, 0.641f, 0.236f, 1.0f, 2.0f, 0.0f, 1.5f);

                static constexpr std::array<std::array<float, 3>, 12> points = {{{28.2f, -5.0f, 2.5f}, {27.8f, -3.4f, 3.8f}, {26.6f, -2.2f, 3.8f}, {25.0f, -1.8f, 2.5f}, {23.4f, -2.2f, 1.2f}, {22.2f, -3.4f, 1.2f}, {21.8f, -5.0f, 2.5f}, {22.2f, -6.6f, 3.8f}, {23.4f, -7.8f, 3.8f}, {25.0f, -8.2f, 2.5f}, {26.6f, -7.8f, 1.2f}, {27.8f, -6.6f, 1.2f}}};
                visualizer.Draw3DPolyline<12>(points, 0.047f, 0.752f, 0.912f, 1.0f, 1.5f, true);

                visualizer.Draw3DPlane(2.0f, 22.0f, 0.0f, 13.0f, 22.0f, 0.0f, 2.0f, 30.0f, 0.0f, 0.047f, 0.752f, 0.912f, 0.4f);
                visualizer.Draw3DPlane(3.0f, 23.0f, 1.0f, 12.0f, 23.0f, 1.0f, 3.0f, 29.0f, 1.0f, 0.047f, 0.752f, 0.912f, 0.4f);
                visualizer.Draw3DPlane(4.0f, 24.0f, 2.0f, 11.0f, 24.0f, 2.0f, 4.0f, 28.0f, 2.0f, 0.047f, 0.752f, 0.912f, 0.4f);

                visualizer.Draw3DDisk<10>(20.0f, 25.0f, 0.0f, 15.0f, 25.0f, 5.0f, 0.783f, 0.899f, 0.121f, 0.4f, 6.0f);
                visualizer.Draw3DDisk<20>(22.0f, 25.0f, 0.0f, 17.0f, 25.0f, 5.0f, 0.783f, 0.899f, 0.121f, 0.4f, 7.0f);
                visualizer.Draw3DDisk<40>(24.0f, 25.0f, 0.0f, 19.0f, 25.0f, 5.0f, 0.783f, 0.899f, 0.121f, 0.4f, 8.0f);

                visualizer.Draw3DBox(-5.0f, 5.0f, -4.0f, -1.228764f, 5.0f, -1.333333f, -6.885618f, 8.265986f, -1.333333f, -6.885618f, 1.734014f, -1.333333f, 0.049f, 0.312f, 0.768f, 0.5f);
                visualizer.Draw3DBox(-5.0f, 5.0f, -3.0f, -2.171573f, 5.0f, -1.0f, -6.414214f, 7.449490f, -1.0f, -6.414214f, 2.550510f, -1.0f, 0.403f, 0.498f, 0.522f, 1.0f);
                visualizer.Draw3DBox(-5.0f, 15.0f, -5.0f, -0.285955f, 15.0f, 0.0f, -7.357023f, 19.082483f, 0.0f, -7.357023f, 10.917517f, 0.0f, 0.741f, 0.112f, 0.389f, 1.0f);

                visualizer.Draw3DCone<32>(-5.0f, -5.0f, -5.0f, -5.0f, -5.0f, 10.0f, 0.093f, 0.834f, 0.512f, 0.5f, 10.0f);
                visualizer.Draw3DCone<32>(-5.0f, -5.0f, 2.5f, -5.0f, -5.0f, -4.5f, 0.874f, 0.561f, 0.128f, 1.0f, 4.0f);
                visualizer.Draw3DCone<16>(-5.0f, 27.0f, -5.0f, -5.0f, 27.0f, 10.0f, 0.618f, 0.245f, 0.789f, 1.0f, 8.0f);

                visualizer.Draw3DArrow<6>(35.0f, -3.0f, -5.0f, 35.0f, -3.0f, 5.0f, 0.742f, 0.895f, 0.234f, 1.0f, 1.0f);
                visualizer.Draw3DArrow<12>(35.0f, -13.0f, -5.0f, 31.0f, -3.0f, 5.0f, 0.476f, 0.793f, 0.109f, 1.0f, 0.5f);
                visualizer.Draw3DArrow<18>(31.0f, -13.0f, -5.0f, 31.0f, -13.0f, 10.0f, 0.912f, 0.187f, 0.356f, 1.0f, 1.5f);

                visualizer.Draw3DAxes<12>(-14.0f, -14.0f, -5.0f, 1.0f, 15.0f, 0.5f);

                visualizer.Draw3DWireframe<8>(-10.0f, 10.0f, -5.0f, 0.0f, 10.0f, -5.0f, -10.0f, 20.0f, -5.0f, -10.0f, 10.0f, 10.0f, 0.822f, 0.899f, 0.315f, 1.0f, 0.25f);
                visualizer.Draw3DWireframe<8>(-9.0f, 23.0f, -5.0f, -1.0f, 23.0f, -5.0f, -9.0f, 31.0f, -5.0f, -9.0f, 23.0f, 10.0f, 0.822f, 0.899f, 0.315f, 1.0f, 0.25f);

                visualizer.Draw3DGrid<8>(23.0f, -14.0f, -5.0f, 21.0f, -14.0f, -5.0f, 22.75f, -13.0f, -5.0f, 0.082f, 0.493f, 0.876f, 1.0f, 0.1f, 6, 8);


                visualizer.InputBasedFreeMovementCameraUpdate(cameraSpeed, cameraSensitivity);


                visualizer.DrawFrame();
            }
        }
        catch (const std::exception& exception)
        {
            std::cerr << "Exception <" << exception.what() << "> was caught." << std::endl;
        }
    }
    else
    {
        visualizer.SetInitialVisualizerMode(true);
        visualizer.SetInitialRendererName("VulkanVisualizer");
        visualizer.SetInitialWindowMode(VulkanWindowMode::Window, 1280, 720);
        visualizer.SetInitialMaxVertexCount(100000);
        visualizer.SetInitialUseMSAA(true, 8);
        visualizer.SetInitialShowLiveTimingInfo(false);
        visualizer.SetInitialShowLiveFrameInfo(false);
        visualizer.SetInitialShowPostCloseTimingInfo(true);

        float Z = 0.0f;

        try
        {
            visualizer.Init();

            visualizer.SetBackgroundColor(0.85f, 0.85f, 0.85f);

            while (visualizer.WindowShouldClose() == false)
            {
                visualizer.HandleFrameStart();

                if (visualizer.IsKeyJustPressed(GLFW_KEY_F1) == true)
                {
                    visualizer.SetWindowMode(VulkanWindowMode::Window);
                }
                else if (visualizer.IsKeyJustPressed(GLFW_KEY_F2) == true)
                {
                    visualizer.SetWindowMode(VulkanWindowMode::BorderlessFullscreen);
                }
                else if (visualizer.IsKeyJustPressed(GLFW_KEY_F3) == true)
                {
                    visualizer.SetWindowMode(VulkanWindowMode::ExclusiveFullscreen);
                }


                Z = 0.0f;

                visualizer.Draw2DTriangle(-900.0f, 0.0f, -900.0f, 480.0f, -60.0f, 480.0f, Z++, 0.891f, 0.234f, 0.618f, 1.0f);
                visualizer.Draw2DTriangle(-900.0f + 100.0f, 0.0f, -900.0f + 100.0f, 480.0f - 100.0f, -60.0f, 480.0f - 100.0f, Z++, 0.047f, 0.589f, 0.912f, 1.0f);
                visualizer.Draw2DTriangle(-900.0f + 200.0f, 0.0f, -900.0f + 200.0f, 480.0f - 200.0f, -60.0f, 480.0f - 200.0f, Z++, 0.754f, 0.812f, 0.176f, 1.0f);
                visualizer.Draw2DTriangle(-900.0f + 300.0f, 0.0f, -900.0f + 300.0f, 480.0f - 300.0f, -60.0f, 480.0f - 300.0f, Z++, 0.398f, 0.925f, 0.314f, 1.0f);
                visualizer.Draw2DTriangle(-846.0f, 243.0f, -254.0f, 31.0f, -465.0f, 440.0f, Z++, 0.582f, 0.147f, 0.835f, 0.5f);

                visualizer.Draw2DPoint<5>(-170.0f, 71.0f, Z++, 0.285f, 0.456f, 0.941f, 1.0f, 50.0f);
                visualizer.Draw2DPoint<10>(-511.0f, 276.0f, Z++, 0.216f, 0.694f, 0.873f, 1.0f, 100.0f);
                visualizer.Draw2DPoint<20>(-386.0f, 366.0f, Z++, 0.512f, 0.534f, 0.551f, 1.0f, 150.0f);
                visualizer.Draw2DPoint<40>(-780.0f, 200.0f, Z++, 0.923f, 0.612f, 0.154f, 1.0f, 200.0f);

                visualizer.Draw2DLineSegment(480.0f, 60.0f, 480.0f, 480.0f, Z++, 0.5f, 0.0f, 1.0f, 1.0f, 40.0f);
                visualizer.Draw2DLineSegment(60.0f, 270.0f, 900.0f, 270.0f, Z++, 0.5f, 0.0f, 1.0f, 1.0f, 40.0f);
                visualizer.Draw2DLineSegment(480.0f, 270.0f, 200.0f, 100.0f, Z++, 0.0f, 0.4f, 1.0f, 0.5f, 100.0f);
                visualizer.Draw2DLineSegment(480.0f, 270.0f, 200.0f, 100.0f, Z++, 1.0f, 1.0f, 1.0f, 1.0f, 10.0f);
                visualizer.Draw2DLineSegment(480.0f, 270.0f, 760.0f, 100.0f, Z++, 0.0f, 1.0f, 0.6f, 0.5f, 100.0f);
                visualizer.Draw2DLineSegment(480.0f, 270.0f, 760.0f, 100.0f, Z++, 1.0f, 1.0f, 1.0f, 1.0f, 10.0f);
                visualizer.Draw2DLineSegment(480.0f, 270.0f, 760.0f, 440.0f, Z++, 1.0f, 0.0f, 0.4f, 0.5f, 100.0f);
                visualizer.Draw2DLineSegment(480.0f, 270.0f, 760.0f, 440.0f, Z++, 1.0f, 1.0f, 1.0f, 1.0f, 10.0f);
                visualizer.Draw2DLineSegment(480.0f, 270.0f, 200.0f, 440.0f, Z++, 1.0f, 0.7f, 0.0f, 0.5f, 100.0f);
                visualizer.Draw2DLineSegment(480.0f, 270.0f, 200.0f, 440.0f, Z++, 1.0f, 1.0f, 1.0f, 1.0f, 10.0f);

                visualizer.Draw2DRectangle(-900.0f, -60.0f, -370.0f, -310.0f, Z++, 0.432f, 0.815f, 0.137f, 1.0f);
                visualizer.Draw2DRectangle(-590.0f, -230.0f, -60.0f, -480.0f, Z++, 0.548f, 0.529f, 0.573f, 1.0f);
                visualizer.Draw2DParallelogram(-860.0f, -440.0f, -860.0f + 300.0f, -440.0f, -860.0f + 150.0f, -440.0f + 150.0f, Z++, 0.0f, 0.2f, 0.5f, 0.6f);
                visualizer.Draw2DParallelogram(-100.0f, -100.0f, -100.0f - 300.0f, -100.0f, -100.0f - 150.0f, -100.0f - 150.0f, Z++, 0.0f, 0.8f, 0.7f, 0.5f);

                visualizer.Draw2DArrow(-60.0f - 210.0f, -480.0f + 105.0f, -900.0f + 210.0f, -480.0f + 210.0f, Z++, 0.0f, 0.0f, 1.0f, 1.0f, 10.0f);
                visualizer.Draw2DArrow(-60.0f - 210.0f, -480.0f + 105.0f + 105.0f, -900.0f + 210.0f, -480.0f + 210.0f + 105.0f, Z++, 0.0f, 0.0f, 1.0f, 1.0f, 10.0f);
                visualizer.Draw2DArrow(-860.0f + 150.0f, -440.0f + 30.0f, -100.0f - 150.0f, -100.0f - 30.0f, Z++, 1.0f, 0.0f, 0.0f, 1.0f, 20.0f);

                visualizer.Draw2DWireframe<16>(60.0f, -480.0f, 900.0f, -480.0f, 60.0f, -60.0f, Z++, 0.091f, 0.318f, 0.964f, 1.0f, 10.0f);

                visualizer.Draw2DGrid<16>(80.0f, -540.0f + 80.0f, 80.0f + 100.0f, -540.0f + 80.0f, 105.0f, -540.0f + 80.0f + 95.0f, Z++, 0.143f, 0.714f, 0.006f, 1.0f, 7.0f, 7, 4);

                static constexpr std::array<std::array<float, 2>, 12> points = {{{480.0f, -90.0f}, {535.4f, -214.6f}, {750.0f, -160.0f}, {595.4f, -270.0f}, {750.0f, -380.0f}, {535.4f, -325.4f}, {480.0f, -450.0f}, {424.6f, -325.4f}, {210.0f, -380.0f}, {364.6f, -270.0f}, {210.0f, -160.0f}, {424.6f, -214.6f}}};
                visualizer.Draw2DPolyline(points, Z++, 0.350f, 0.020f, 0.050f, 1.0f, 30.0f, true);

                visualizer.Draw2DAxes(0.0f, 0.0f, 3.5f, 400.f, 10.0f, 1.0f);


                visualizer.DrawFrame();
            }
        }
        catch (const std::exception& exception)
        {
            std::cerr << "Exception <" << exception.what() << "> was caught." << std::endl;
        }
    }

    return 0;
}

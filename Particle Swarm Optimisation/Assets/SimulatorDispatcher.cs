using System.Collections;
using System.Collections.Generic;
using UnityEngine.Experimental.Rendering;
using UnityEngine;

/// <summary>
/// Handles dispatching initialisation data and commands to the simulation compute shader.
/// </summary>
public class SimulatorDispatcher : MonoBehaviour
{
    public ComputeShader simulator;

    private const int OutputWidth = 1024;

    private const int OutputHeight = 1024;

    private const int XThreads = 8;

    private const int YThreads = 8;

    private RenderTexture _renderTarget;

#if UNITY_EDITOR
    private void Start() 
    {
        if (!simulator)
        {
            Debug.LogError("No simulator was provided. Interrupting...");
            throw new System.Exception();
        }
    }
#endif

    private void Awake() 
    {
        _renderTarget = new RenderTexture(OutputWidth, OutputHeight, 32, DefaultFormat.LDR);
        _renderTarget.enableRandomWrite = true;
        _renderTarget.Create();

        simulator.SetTexture(0, Shader.PropertyToID("Result"), _renderTarget, 0);
        simulator.SetInt(Shader.PropertyToID("OutputWidth"), OutputWidth);
        simulator.SetInt(Shader.PropertyToID("OutputHeight"), OutputHeight);
    }

    private void OnRenderImage(RenderTexture src, RenderTexture dest) 
    {
        simulator.Dispatch(0, OutputWidth / XThreads, OutputHeight / YThreads, 1);
        Graphics.Blit(_renderTarget, dest);
    }
}
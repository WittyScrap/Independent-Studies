using System.Collections;
using System.Collections.Generic;
using UnityEngine.Experimental.Rendering;
using UnityEngine;

/// <summary>
/// Handles dispatching initialisation data and commands to the simulation compute shader.
/// </summary>
public class SimulatorDispatcher : MonoBehaviour
{
    /// <summary>
    /// Represents a single particle for the Particle Swarm Optimisation
    ///  based algorithm.
    /// </summary>
    private struct Particle
    {
        Vector2Int position;

        Vector2 velocity;
    }

    /// <summary>
    /// Represents a single particle for the Bees based algorithm.
    /// </summary>
    private struct Bee
    {
        Vector2Int position;

        Vector2 velocity;
    }

    public ComputeShader simulator;

    private const int OutputWidth = 1024;

    private const int OutputHeight = 1024;

    private const int ParticlesCount = 16384;

    private RenderTexture _particleSpace;

    private int _csPrepare;

    private int _csParticles;

    private Particle[] _psoParticles = new Particle[ParticlesCount];

    private Bee[] _beeParticles = new Bee[ParticlesCount];


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
        var particlesBuffer = new ComputeBuffer(ParticlesCount, sizeof(int) * 2 + sizeof(float) * 2, 
                                    ComputeBufferType.Structured);

        particlesBuffer.SetData(_psoParticles);

        _particleSpace = new RenderTexture(OutputWidth, OutputHeight, 4, DefaultFormat.LDR);
        _particleSpace.enableRandomWrite = true;
        _particleSpace.Create();

        simulator.SetInt(Shader.PropertyToID("OutputWidth"), OutputWidth);
        simulator.SetInt(Shader.PropertyToID("OutputHeight"), OutputHeight);

        _csParticles = simulator.FindKernel("CSParticles");
        _csPrepare = simulator.FindKernel("CSPrepare");
        
        // Setup resources for all kernels
        simulator.SetBuffer(_csPrepare, "Particles", particlesBuffer);
        simulator.SetBuffer(_csParticles, "Particles", particlesBuffer);
        simulator.SetTexture(_csParticles, "ParticleSpace", _particleSpace, 0);

        ResetValues();

        particlesBuffer.Release();
    }

    private void ResetValues()
    {
        simulator.Dispatch(_csPrepare, ParticlesCount / 1024, 1, 1);
        
        simulator.Dispatch(
            _csParticles, 
            OutputWidth     / 160,
            OutputHeight    / 160,
            ParticlesCount  / 1024
        );
    }

    private void OnRenderImage(RenderTexture src, RenderTexture dest) 
    {
        Graphics.Blit(_particleSpace, dest);
    }
}
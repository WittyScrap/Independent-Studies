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
    /// based algorithm.
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

    private int _csSimulate;

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

    private unsafe void Awake() 
    {
        _particleSpace = new RenderTexture(OutputWidth, OutputHeight, 4, DefaultFormat.LDR);
        _particleSpace.enableRandomWrite = true;
        _particleSpace.Create();

        simulator.SetInt("OutputWidth", OutputWidth);
        simulator.SetInt("OutputHeight", OutputHeight);

        _csPrepare = simulator.FindKernel("CSPrepare");
        _csSimulate = simulator.FindKernel("CSSimulate");

        var buff = new ComputeBuffer(ParticlesCount, sizeof(Particle), ComputeBufferType.Structured);
        buff.SetData(_psoParticles);
        
        simulator.SetBuffer(_csPrepare, "Particles", buff);
        simulator.SetTexture(_csSimulate, "ParticleSpace", _particleSpace, 0);
        simulator.Dispatch(_csPrepare, ParticlesCount / 1024, 1, 1);

        buff.GetData(_psoParticles);
        buff.Release();
    }

    private unsafe void Update()
    {
        var buff = new ComputeBuffer(ParticlesCount, sizeof(Particle), ComputeBufferType.Structured);
        buff.SetData(_psoParticles);

        simulator.SetBuffer(_csSimulate, "Particles", buff);
        simulator.Dispatch(_csSimulate, ParticlesCount / 1024, 1, 1);

        buff.GetData(_psoParticles);
        buff.Release();
    }

    private void OnRenderImage(RenderTexture src, RenderTexture dest) 
    {
        Graphics.Blit(_particleSpace, dest);
    }
}
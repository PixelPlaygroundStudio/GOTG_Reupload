#include "wipch.h"
#include "ParticleSystem.h"
#include "glew.h"
#include "Wiwa/ecs/components/Mesh.h"
#include <Wiwa/core/Renderer3D.h>

#include <Wiwa/ecs/EntityManager.h>
#include <Wiwa/utilities/render/Material.h>
#include <Wiwa/utilities/render/CameraManager.h>
#include <Wiwa/utilities/render/LightManager.h>
namespace Wiwa {
	ParticleSystem::ParticleSystem()
	{
	}
	ParticleSystem::~ParticleSystem()
	{
	}
	void ParticleSystem::OnSystemAdded()
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}
		SetEmitterBools(emitter);

		Transform3D* t3d = GetComponent<Transform3D>();

		ResourceId meshid = Wiwa::Resources::Load<Model>(emitter->m_meshPath);
		m_Model = Wiwa::Resources::GetResourceById<Model>(meshid);

		ResourceId matid = Wiwa::Resources::Load<Material>(emitter->m_materialPath);
		m_Material = Wiwa::Resources::GetResourceById<Material>(matid);

		m_MaxParticles = emitter->m_maxParticles;
		emitter->m_meshChanged = false;
		emitter->m_materialChanged = false;

		m_SpawnTimer = 0;
		emitter->m_spawnTimer = 0;
		m_AlreadySpawned = false;
		
		m_AvailableParticles = emitter->m_maxParticles;

		emitter->m_activeTimeChanged = false;

		glm::vec3 initPosition = emitter->m_p_initialPosition + t3d->position;
		glm::vec3 initRotation = emitter->m_p_initialRotation + t3d->position;
		glm::vec3 initScale = emitter->m_p_initialScale + t3d->localScale;
		glm::vec3 initZero(0, 0, 0);

		// init particle struct
		m_Particles.resize(m_MaxParticles);
		for (uint i = 0; i < m_MaxParticles; i++)
		{
			m_Particles[i] = Particle(0, initZero, initZero, initZero, initZero, initZero, initZero, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if ((int)emitter->m_p_positionFollowsRotation > 1 || (int)emitter->m_p_positionFollowsRotation < 0)
			emitter->m_p_positionFollowsRotation = false;

		if ((int)emitter->m_useAdditiveBlending > 1 || (int)emitter->m_useAdditiveBlending < 0)
			emitter->m_useAdditiveBlending = false;


		//start

		if (m_Model == nullptr)
		{
			WI_CORE_ERROR("ParticleSystem: Error loading Mesh at path:'{0}', Entity id: {1}", emitter->m_meshPath, (int)m_EntityId);
		}
		if (m_Material == nullptr)
		{
			WI_CORE_ERROR("ParticleSystem: Error loading Material at path:'{0}, Entity id: {1}'", emitter->m_materialPath, (int)m_EntityId);
		}



		if (emitter->m_p_rangedSpawnDelay)
		{
			m_SpawnTimer = Wiwa::Math::RandomRange(emitter->m_p_minSpawnDelay, emitter->m_p_maxSpawnDelay);

		}
		else
		{

			m_SpawnTimer = emitter->m_p_spawnDelay;
		}
		emitter->m_spawnTimer = m_SpawnTimer;

		if (emitter->m_activeOverTime)
		{
			if (emitter->m_startActive)
			{
				if (emitter->m_rangedTimeActive)
				{
					emitter->m_ActiveTimer = Wiwa::Math::RandomRange(emitter->m_minInitialTimeActive, emitter->m_maxInitialTimeActive);
				}
				else
				{
					emitter->m_ActiveTimer = emitter->m_initialTimeActive + emitter->m_p_spawnDelay;
				}
			}
			else
			{
				emitter->m_ActiveTimer = 0;
			}

		}
		if (m_SpawnTimer <= 0 && (emitter->m_active || emitter->m_activeOverTime))
			SpawnParticleSet();

	}
	void ParticleSystem::OnAwake()
	{
	}
	void ParticleSystem::OnInit()
	{

	}
	bool ParticleSystem::OnEnabledFromPool()
	{
		RestartEmitter();
		return true;
	}
	void ParticleSystem::OnUpdate()
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			return;
		}
		float dt = Time::GetDeltaTime() * 0.001;

		if (emitter->m_meshChanged)
		{
			ResourceId meshid = Wiwa::Resources::Load<Model>(emitter->m_meshPath);
			m_Model = Wiwa::Resources::GetResourceById<Model>(meshid);
			emitter->m_meshChanged = false;

		}

		if (emitter->m_materialChanged)
		{
			ResourceId matid = Wiwa::Resources::Load<Material>(emitter->m_materialPath);
			m_Material = Wiwa::Resources::GetResourceById<Material>(matid);
			emitter->m_materialChanged = false;

		}

		if (emitter->m_activeTimeChanged && emitter->m_startActive)
		{
			if (emitter->m_activeOverTime)
			{
				if (emitter->m_rangedTimeActive)
				{
					emitter->m_ActiveTimer = Wiwa::Math::RandomRange(emitter->m_minInitialTimeActive, emitter->m_maxInitialTimeActive);
				}
				else
				{
					emitter->m_ActiveTimer = emitter->m_initialTimeActive;
				}
			}
			emitter->m_activeTimeChanged = false;
		}
		
		if (m_MaxParticles != emitter->m_maxParticles)
		{
			glm::vec3 initZero(0, 0, 0);

			m_Particles.clear();
			m_MaxParticles = emitter->m_maxParticles;
			
			m_Particles.resize(m_MaxParticles);
			
			for (uint i = 0; i < m_MaxParticles; i++)
			{
				m_Particles[i] = Particle(0, initZero, initZero, initZero, initZero, initZero, initZero, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}

		if ((emitter->m_active || emitter->m_activeOverTime) /*&& !Time::IsPaused()*/)
		{
			

			if (emitter->m_activeOverTime)
			{
				
				emitter->m_ActiveTimer -= dt;
			}

			if (m_SpawnTimer > 0 )
			{
				m_SpawnTimer -= dt;
				emitter->m_spawnTimer = m_SpawnTimer;
			}



			if (emitter->m_activeOverTime)
			{
				if (emitter->m_ActiveTimer <= 0 && emitter->m_destroyOnFinishActive && Time::IsPlaying() )
				{
					if (emitter->m_destroyActiveParticles)
					{
						m_Particles.clear();

						EntityManager& eman = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
						//WI_CORE_INFO("active timer over");
						eman.DestroyEntity(m_EntityId);
					}
					else if (emitter->m_activeParticles == 0)
					{
						EntityManager& eman = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
						//WI_CORE_INFO("active timer over");
						eman.DestroyEntity(m_EntityId);
					}
					

					
				}


				if (emitter->m_active && emitter->m_ActiveTimer <= 0)
				{
					if (emitter->m_rangedTimeActive)
					{
						emitter->m_ActiveTimer = Wiwa::Math::RandomRange(emitter->m_minInitialTimeActive, emitter->m_maxInitialTimeActive);
					}
					else
					{
						emitter->m_ActiveTimer = emitter->m_initialTimeActive;
					}
				}

				if (m_SpawnTimer <= 0 && emitter->m_ActiveTimer > 0)
				{
					if (!m_AlreadySpawned)
						SpawnParticleSet();

					if (emitter->m_loopSpawning)
					{
						m_SpawnTimer = emitter->m_spawnRate;

						if (emitter->m_p_rangedSpawnRate)
						{
							m_SpawnTimer = Wiwa::Math::RandomRange(emitter->m_p_minSpawnRate, emitter->m_p_maxSpawnRate);
						}
					}
					else
					{
						m_AlreadySpawned = true;
					}
					
				}
				else
				{
					emitter->m_active = false;
				}
			}
			else
			{
				if (m_SpawnTimer <= 0)
				{
					if (!m_AlreadySpawned)
					{
						SpawnParticleSet();
						m_AlreadySpawned = true;
					}
						

					if (emitter->m_loopSpawning)
					{
						m_AlreadySpawned = false;

						m_SpawnTimer = emitter->m_spawnRate;

						if (emitter->m_p_rangedSpawnRate)
						{
							m_SpawnTimer = Wiwa::Math::RandomRange(emitter->m_p_minSpawnRate, emitter->m_p_maxSpawnRate);
						}
					}
				}
			}



			//WI_CORE_INFO("spawn timer = {0}", m_SpawnTimer);

		}


		int activeParticles = 0;

		if (!Time::IsPaused())
			activeParticles = 0;

		for (unsigned int i = 0; i < m_MaxParticles && !m_Particles.empty(); ++i)
		{
			Particle& particle = m_Particles[i];

			if (particle.life_time > 0.0f || emitter->m_permanentParticles)
			{
				/*if (!Time::IsPaused())*/
				{
					if (!emitter->m_permanentParticles || (emitter->m_permanentParticles && emitter->m_cycleLifeTime))
					{
						UpdateParticleLife(particle, dt);

					}

					activeParticles++;

					glm::vec4 color(0);

					if (emitter->m_useNewColorNodes)
					{
						if (emitter->m_newColorsUsed > 1 && emitter->m_newColorsUsed <= 20)
						{
							/*std::string perc = "particle life percentage: " + std::to_string(particle.life_percentage);
							WI_CORE_INFO(perc.c_str());*/

							for (size_t j = 0; j < emitter->m_newColorsUsed - 1; j++)
							{
								if (particle.life_percentage >= emitter->m_newColorsNodes[j].m_percentage * 0.01f &&
									particle.life_percentage < emitter->m_newColorsNodes[j + 1].m_percentage * 0.01f)
								{
									/*std::string cid = std::to_string(j) + " Color id: " + std::to_string(j);
									WI_CORE_INFO(cid.c_str());*/

									color = particle.color = InterpolateVec4(
										emitter->m_newColorsNodes[j].color,
										emitter->m_newColorsNodes[j + 1].color,
										particle.life_percentage - emitter->m_newColorsNodes[j].m_percentage * 0.01f,
										emitter->m_newColorsNodes[j + 1].m_percentage * 0.01f - emitter->m_newColorsNodes[j].m_percentage * 0.01f
									);
								}
							}
						}
						else
						{
							color = particle.color = emitter->m_newColorsNodes[0].color;
						}
					}
					else
					{
						if (emitter->m_colorsUsed > 1 && emitter->m_colorsUsed <= 4)
						{
							/*std::string perc = "particle life percentage: " + std::to_string(particle.life_percentage);
							WI_CORE_INFO(perc.c_str());*/

							for (size_t j = 0; j < emitter->m_colorsUsed - 1; j++)
							{
								if (particle.life_percentage >= emitter->m_p_colorsOverLifetime[j].m_percentage * 0.01f &&
									particle.life_percentage < emitter->m_p_colorsOverLifetime[j + 1].m_percentage * 0.01f)
								{
									/*std::string cid = std::to_string(j) + " Color id: " + std::to_string(j);
									WI_CORE_INFO(cid.c_str());*/

									color = particle.color = InterpolateVec4(
										emitter->m_p_colorsOverLifetime[j].color,
										emitter->m_p_colorsOverLifetime[j + 1].color,
										particle.life_percentage - emitter->m_p_colorsOverLifetime[j].m_percentage * 0.01f,
										emitter->m_p_colorsOverLifetime[j + 1].m_percentage * 0.01f - emitter->m_p_colorsOverLifetime[j].m_percentage * 0.01f
									);
								}
							}
						}
						else
						{
							color = particle.color = emitter->m_p_colorsOverLifetime[0].color;
						}
					}

					

					if (m_Material)
					{
						Uniform* u_color = m_Material->getUniform("u_Color");
					if (u_color != nullptr)
						u_color->setData(color, UniformType::fVec4);

					Uniform* u_life = m_Material->getUniform("u_LifeTime");
					if (u_life != nullptr)
						u_life->setData(particle.life_percentage, UniformType::Float);

					Uniform* u_Time = m_Material->getUniform("u_Time");
					if (u_Time != nullptr)
						u_Time->setData(Wiwa::Time::GetRealTimeSinceStartup(), UniformType::Float);
					}
					


					//calculate everything
					Transform3D* t3d = GetComponent<Transform3D>();

					if (emitter->m_p_positionTowardsPoint)
					{
						particle.transform.localPosition = (particle.startPosition) * (1 - particle.life_percentage) + (t3d->position + emitter->m_p_positionTowardsPointPos) * particle.life_percentage;
					}
					else
					{
						glm::vec3 resultantVel = particle.velocity;
						
						if (emitter->m_p_followEmitterRotation)
						{
							glm::vec3 rotationVelRad = glm::radians(particle.transform.localRotation);

							resultantVel = glm::rotate(resultantVel, rotationVelRad.x, glm::vec3(1.0f, 0.0f, 0.0f));
							resultantVel = glm::rotate(resultantVel, rotationVelRad.y, glm::vec3(0.0f, 1.0f, 0.0f));
							resultantVel = glm::rotate(resultantVel, rotationVelRad.z, glm::vec3(0.0f, 0.0f, 1.0f));

						}

						particle.transform.localPosition += resultantVel * dt;
					}

					if (emitter->m_p_useGravity)
					{
						float grav = emitter->m_p_gravity * dt;
						particle.velocity.y -= grav;
					}

					if (emitter->m_p_scaleOverTime)
					{

						//particle.scale = emitter->m_p_scaleOverTimeStart * (1 - particle.life_percentage) + emitter->m_p_scaleOverTimeEnd * particle.life_percentage * emitter->m_p_growTime;
						if (particle.life_percentage > emitter->m_p_scaleOverTimePerStart * 0.01f &&
							particle.life_percentage < emitter->m_p_scaleOverTimePerEnd * 0.01f)
						{
							particle.transform.localScale = InterpolateVec3(
								emitter->m_p_scaleOverTimeStart,
								emitter->m_p_scaleOverTimeEnd,
								particle.life_percentage - emitter->m_p_scaleOverTimePerStart * 0.01f,
								emitter->m_p_scaleOverTimePerEnd * 0.01f - emitter->m_p_scaleOverTimePerStart * 0.01f
							);
							//WI_CORE_INFO("[{0}] interpolating ", particle.life_percentage);
						}
						else if (particle.life_percentage < emitter->m_p_scaleOverTimePerStart * 0.01f)
						{
							//WI_CORE_INFO("[{0}] clamp to start scale", particle.life_percentage);
							particle.transform.localScale = emitter->m_p_scaleOverTimeStart;
						}
						else if (particle.life_percentage > emitter->m_p_scaleOverTimePerEnd * 0.01f)
						{
							//WI_CORE_INFO("[{0}] clamp to end scale", particle.life_percentage);
							particle.transform.localScale = emitter->m_p_scaleOverTimeEnd;
						}


					}
					else
					{
						//calculate size X
						if (emitter->m_stopSizeAtZero && emitter->m_stopSizeAtZeroX)
						{
							if (particle.transform.localScale.x + particle.growthVelocity.x * dt > 0)
							{
								particle.transform.localScale.x += particle.growthVelocity.x * dt;
							}
							else
							{
								particle.transform.localScale.x = 0;
							}
						}
						else
						{
							particle.transform.localScale.x += particle.growthVelocity.x * dt;
						}

						//calculate size Y
						if (emitter->m_stopSizeAtZero && emitter->m_stopSizeAtZeroY)
						{
							if (particle.transform.localScale.y + particle.growthVelocity.y * dt > 0)
							{
								particle.transform.localScale.y += particle.growthVelocity.y * dt;
							}
							else
							{
								particle.transform.localScale.y = 0;
							}
						}
						else
						{
							particle.transform.localScale.y += particle.growthVelocity.y * dt;
						}

						//calculate size Z
						if (emitter->m_stopSizeAtZero && emitter->m_stopSizeAtZeroZ)
						{
							if (particle.transform.localScale.z + particle.growthVelocity.z * dt > 0)
							{
								particle.transform.localScale.z += particle.growthVelocity.z * dt;
							}
							else
							{
								particle.transform.localScale.z = 0;
							}
						}
						else
						{
							particle.transform.localScale.z += particle.growthVelocity.z * dt;
						}

					}

					if (emitter->m_p_rotationOverTime)
					{
						//particle.rotation = emitter->m_p_rotationOverTimeStart * (1 - particle.life_percentage) + emitter->m_p_rotationOverTimeEnd * particle.life_percentage * emitter->m_p_growTime;
						if (particle.life_percentage > emitter->m_p_rotationOverTimePerStart * 0.01f &&
							particle.life_percentage < emitter->m_p_rotationOverTimePerEnd * 0.01f)
						{
							particle.transform.localRotation = InterpolateVec3(
								emitter->m_p_rotationOverTimeStart,
								emitter->m_p_rotationOverTimeEnd,
								particle.life_percentage - emitter->m_p_rotationOverTimePerStart * 0.01f,
								emitter->m_p_rotationOverTimePerEnd * 0.01f - emitter->m_p_rotationOverTimePerStart * 0.01f
							);
						}
						else if (particle.life_percentage < emitter->m_p_rotationOverTimePerStart * 0.01f)
						{
							particle.transform.localRotation = emitter->m_p_rotationOverTimeStart;
						}
						else if (particle.life_percentage > emitter->m_p_rotationOverTimePerEnd * 0.01f)
						{
							particle.transform.localRotation = emitter->m_p_rotationOverTimeEnd;

						}
					}
					else
					{
						particle.transform.localRotation += particle.angularVelocity * dt;
					}



					//update world space values
					if (emitter->m_p_followEmitterRotation)
					{
						particle.transform.rotation = t3d->rotation + particle.transform.localRotation;

					}
					else
					{
						particle.transform.rotation = particle.transform.localRotation;

					}

					glm::mat4 rotMat(1);
					glm::vec3 rotationRad = glm::radians(particle.transform.rotation);
					glm::vec3 orginalPos(0);




					
					orginalPos = particle.transform.localPosition;

					if (emitter->m_p_positionFollowsRotation)
					{
						orginalPos = glm::rotate(orginalPos, rotationRad.x, glm::vec3(1.0f, 0.0f, 0.0f));
						orginalPos = glm::rotate(orginalPos, rotationRad.y, glm::vec3(0.0f, 1.0f, 0.0f));
						orginalPos = glm::rotate(orginalPos, rotationRad.z, glm::vec3(0.0f, 0.0f, 1.0f));

					}

					if (emitter->m_p_followEmitterPosition)
						orginalPos += t3d->position;


					particle.transform.position = orginalPos;

					particle.transform.scale = particle.transform.localScale;



					// Create transformation matrix
					glm::mat4 transform = glm::mat4(1.0f);

					if (emitter->m_billboardActive)
					{
						Wiwa::CameraManager& cm = Wiwa::SceneManager::getActiveScene()->GetCameraManager();
						CameraId cameraId;
						cameraId = cm.getActiveCameraId();
						Wiwa::Camera* cam = cm.getCamera(cameraId);


						glm::mat4 rotationMatrix = cam->getView();

						glm::vec3 rotationRad = glm::vec3(0, 0, 0);
						rotationRad.x = -glm::asin(rotationMatrix[1][2]);
						rotationRad.y = glm::atan(rotationMatrix[0][2], rotationMatrix[2][2]);
						rotationRad.z = glm::atan(rotationMatrix[1][0], rotationMatrix[1][1]);

						transform = glm::translate(transform, particle.transform.position);

						//transform = billboardMatrix * transform;

						

						transform = glm::rotate(transform, rotationRad.y, glm::vec3(0.0f, 1.0f, 0.0f));
						transform = glm::rotate(transform, rotationRad.z, glm::vec3(0.0f, 0.0f, 1.0f));
						transform = glm::rotate(transform, rotationRad.x, glm::vec3(1.0f, 0.0f, 0.0f));

						if (!emitter->m_billboardLockAxisY)
						transform = glm::rotate(transform, glm::radians(particle.transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
						if (!emitter->m_billboardLockAxisZ)
							transform = glm::rotate(transform, glm::radians(particle.transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
						if (!emitter->m_billboardLockAxisX)
							transform = glm::rotate(transform, glm::radians(particle.transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

						//debug
						rotationRad.x = glm::degrees(glm::asin(rotationMatrix[1][2]));
						rotationRad.y = glm::degrees(glm::atan(rotationMatrix[0][2], rotationMatrix[2][2]));
						rotationRad.z = glm::degrees(glm::atan(rotationMatrix[1][0], rotationMatrix[1][1]));
						
						transform = glm::scale(transform, particle.transform.scale);

						//pass transformation matrix
						particle.transform.worldMatrix = transform;

					}
					else
					{
						// Convert rotation angles from degrees to radians
						glm::vec3 rotationRad = glm::radians(particle.transform.rotation);

						transform = glm::translate(transform, particle.transform.position);
						transform = glm::rotate(transform, rotationRad.x, glm::vec3(1.0f, 0.0f, 0.0f));
						transform = glm::rotate(transform, rotationRad.y, glm::vec3(0.0f, 1.0f, 0.0f));
						transform = glm::rotate(transform, rotationRad.z, glm::vec3(0.0f, 0.0f, 1.0f));
						transform = glm::scale(transform, particle.transform.scale);
						//pass transformation matrix
						particle.transform.worldMatrix = transform;
					}
				}

				//DEBUG PURPOSES ----------------------------------------------------------------------------

				/*WI_CORE_INFO("Local Matrix: ");

				for (size_t _y = 0; _y < 4; _y++)
				{
					std::string r;
					for (size_t _x = 0; _x < 4; _x++)
					{
						r += std::to_string(particle.transform.localMatrix[_x][_y]) + " ";
					}
					WI_CORE_INFO(r.c_str());
				}
				WI_CORE_INFO("-------------------");

				WI_CORE_INFO("World Matrix: ");

				for (size_t _y = 0; _y < 4; _y++)
				{
					std::string r;
					for (size_t _x = 0; _x < 4; _x++)
					{
						r += std::to_string(particle.transform.worldMatrix[_x][_y]) + " ";
					}
					WI_CORE_INFO(r.c_str());
				}
				WI_CORE_INFO("===================");*/

				if (m_Model != nullptr && m_Material != nullptr)
					Render(particle);
			}
			else if (emitter->m_permanentParticles && emitter->m_cycleLifeTime)
			{
				SetParticleLifeTime(particle, particle.life_time_start);
			}
		}

		emitter->m_activeParticles = activeParticles;
		m_AvailableParticles = emitter->m_maxParticles - activeParticles;

		
		if (emitter->m_ActiveTimer < 0)  emitter->m_ActiveTimer = 0;
	}


	glm::vec4 ParticleSystem::InterpolateVec4(glm::vec4 valStart, glm::vec4 valEnd, float currentPercentage, float targetPercentage)
	{

		float process = currentPercentage / targetPercentage;
		glm::vec4 res = valStart + (valEnd - valStart) * process;

		/*std::string cp = "currentPer: " + std::to_string(currentPercentage);
		std::string tp = "targetPer: " + std::to_string(targetPercentage);
		std::string p = "process: " + std::to_string(process);
		std::string t = "Color: " + std::to_string(color.x) + " " + std::to_string(color.y) + " " + std::to_string(color.z) + " " + std::to_string(color.w);
		
		WI_CORE_INFO(cp.c_str());
		WI_CORE_INFO(tp.c_str());
		WI_CORE_INFO(p.c_str());
		WI_CORE_INFO(t.c_str());*/
		return res;
	}

	float ParticleSystem::InterpolateFloat(float valStart,float valEnd, float currentPercentage, float targetPercentage)
	{

		float process = currentPercentage / targetPercentage;
		float res = valStart + (valEnd - valStart) * process;

		return res;
	}

	glm::vec3 ParticleSystem::InterpolateVec3(glm::vec3 valStart, glm::vec3 valEnd, float currentPercentage, float targetPercentage)
	{

		float process = currentPercentage / targetPercentage;
		glm::vec3 res = valStart + (valEnd - valStart) * process;

		return res;
	}

	void ParticleSystem::EmitParticleBatch(int amount)
	{
		for (unsigned int i = 0; i < (uint)amount; ++i)
		{
			if (m_AvailableParticles > 0)
			{
				int unusedParticle = FirstUnusedParticle();
				SpawnParticle(m_Particles[unusedParticle]);
			}
		}
	}

	
	void ParticleSystem::EmitParticleBatch()
	{
		SpawnParticleSet();
	}

	void ParticleSystem::SetActive(bool active, bool forceDeactivateParticles)
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}
		emitter->m_active = active;

		if (forceDeactivateParticles)
			DeactivateParticles();
	}

	void ParticleSystem::SetMaterial(const char* materialPath)
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}

		if (emitter == nullptr) return;

		strcpy(emitter->m_materialPath, materialPath);
		emitter->m_materialChanged = true;
	}

	void ParticleSystem::SetMesh(const char* meshPath)
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}

		ResourceId meshid = Wiwa::Resources::Load<Model>(meshPath);
		
		if (meshid != -1)
			m_Model = Wiwa::Resources::GetResourceById<Model>(meshid);
		else
			WI_CORE_INFO("Mesh could not be found {0}", meshPath);
	}

	void ParticleSystem::SetTimer(float time)
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}
		m_SpawnTimer = time;
	}

	void ParticleSystem::DeactivateParticles()
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}

		for (unsigned int i = 0; i < m_MaxParticles && !m_Particles.empty(); ++i)
		{
			Particle& particle = m_Particles[i];
			particle.life_time = 0;
			particle.life_percentage = 0;
		}
	}

	void ParticleSystem::RestartEmitter(float timer)
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}
		
		DeactivateParticles();

		if (timer == 0)
			SpawnParticleSet();
		else
			SetTimer(timer);

		if (emitter->m_rangedTimeActive)
		{
			emitter->m_ActiveTimer = Wiwa::Math::RandomRange(emitter->m_minInitialTimeActive, emitter->m_maxInitialTimeActive);
		}
		else
		{
			emitter->m_ActiveTimer = emitter->m_initialTimeActive;
		}

		m_AlreadySpawned = false;
	}

	void ParticleSystem::SetColor(int index, glm::vec4 color)
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}
		
		if (emitter->m_useNewColorNodes)
		{
			if (index >= 0 && index < 20)
			{
				emitter->m_newColorsNodes[index].color = color;
			}
		}
		else
		{
			if (index >= 0 && index < 4)
			{
				emitter->m_newColorsNodes[index].color = color;
			}
		}
	}

	void ParticleSystem::OnDestroy()
	{
		m_Model = nullptr;
		m_Material = nullptr;

		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}
		emitter->m_ActiveTimer = 0;
	}

	bool ParticleSystem::OnDisabledFromPool()
	{
		for (size_t i = 0; i < m_Particles.size(); i++)
		{
			Particle& particle = m_Particles[i];
			particle.life_percentage = 0;
			particle.life_time_start = 0;
			particle.life_time = 0;
		}

		return false;
	}

	void ParticleSystem::Render(Particle& particle)
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}

		Transform3D* t3d = GetComponent<Transform3D>();

		Renderer3D& r3d = Application::Get().GetRenderer3D();
		CameraManager& man = Wiwa::SceneManager::getActiveScene()->GetCameraManager();
		EntityManager& eman = Wiwa::SceneManager::getActiveScene()->GetEntityManager();
		LightManager& lman = Wiwa::SceneManager::getActiveScene()->GetLightManager();

		size_t cameraCount = man.getCameraSize();
		std::vector<CameraId>& cameras = man.getCameras();

		

		
		GL(Enable(GL_CULL_FACE));

		if (emitter->m_deactivateFaceCulling)
		{
			GL(Disable(GL_CULL_FACE));
		}


		GL(Enable(GL_BLEND));
		GL(DepthMask(GL_FALSE));
		
	
		if (emitter->m_useAdditiveBlending)
		{
			GL(BlendFunc(GL_SRC_ALPHA, GL_ONE));
			GL(BlendEquation(GL_FUNC_ADD));
		}
		else if (emitter->m_useMultiplyBlending)
		{
			GL(BlendFunc(GL_DST_COLOR, GL_ZERO));
			GL(BlendEquation(GL_FUNC_ADD));
		}
		else {
			GL(BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
			GL(BlendEquation(GL_FUNC_ADD));
		}




		for (size_t i = 0; i < cameraCount; i++)
		{
			CameraId cam_id = cameras[i];
			Camera* camera = man.getCamera(cam_id);

			if (camera->cull && !camera->frustrum.IsBoxVisible(m_Model->boundingBox.getMin(), m_Model->boundingBox.getMax()))
				return;
			r3d.RenderMesh(
				m_Model,
				particle.transform.worldMatrix, m_Material,
				lman.GetDirectionalLight(),
				lman.GetPointLights(),
				lman.GetSpotLights(),
				false,
				true,
				false, 
				camera
			);
			
		}
		if (man.editorCamera)
		{
			r3d.RenderMesh(
				m_Model, 
				particle.transform.worldMatrix, m_Material, 
				lman.GetDirectionalLight(),
				lman.GetPointLights(),
				lman.GetSpotLights(),
				false,
				true,
				false,
				man.editorCamera
			);
		}

		GL(ActiveTexture(GL_TEXTURE0));
		GL(DepthMask(GL_TRUE));
		GL(Disable(GL_BLEND));
	}

	void ParticleSystem::SetValues(ParticleEmitterComponent settings)
	{

	}

	void ParticleSystem::SetParticleLifeTime(Particle& particle, float lifeTime)
	{
		particle.life_time_start = lifeTime;
		particle.life_time = lifeTime;
		particle.life_percentage = 0.f;
	}
	void ParticleSystem::UpdateParticleLife(Particle& particle, float deltaTime)
	{
		particle.timeAlive += deltaTime;
		particle.life_time -= deltaTime;
		particle.life_percentage = 1 - particle.life_time / particle.life_time_start;
	}

	void ParticleSystem::SpawnParticle(Particle& particle)
	{

		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}

		Transform3D* t3d = GetComponent<Transform3D>();

		glm::vec3 initPosition(0, 0, 0);
		glm::vec3 initRotation(0, 0, 0);
		glm::vec3 initScale(0, 0, 0);

		//initial position


		glm::vec3 followSpawnPos(0);
		glm::vec3 followSpawnRot(0);

		if (emitter->m_p_followEmitterPositionSpawn) followSpawnPos = t3d->position;
		if (emitter->m_p_followEmitterRotationSpawn) followSpawnRot = t3d->rotation;

		switch (emitter->m_spawnVolume)
		{
			case Wiwa::ParticleSpawnVolume::NONE:
			{
				initPosition = emitter->m_p_initialPosition/*+ t3d->position*/;
			}
			break;
			case Wiwa::ParticleSpawnVolume::CUBE:
			{
				float x = Wiwa::Math::RandomRange(emitter->m_p_initialPositionBoxA.x, emitter->m_p_initialPositionBoxB.x);
				float y = Wiwa::Math::RandomRange(emitter->m_p_initialPositionBoxA.y, emitter->m_p_initialPositionBoxB.y);
				float z = Wiwa::Math::RandomRange(emitter->m_p_initialPositionBoxA.z, emitter->m_p_initialPositionBoxB.z);

				initPosition = emitter->m_p_initialPosition /*+ t3d->position*/ + glm::vec3(x, y, z);
			}
			break;
			case Wiwa::ParticleSpawnVolume::SPHERE:
			{
				//Spawn in sphere

				float x = Wiwa::Math::RandomRange(1.0f,-1.0f);
				float y = Wiwa::Math::RandomRange(1.0f,-1.0f);
				float z = Wiwa::Math::RandomRange(1.0f,-1.0f);


				initPosition = emitter->m_p_initialPosition/*t3d->position*/ + emitter->m_p_initialPositionSphCenter + glm::normalize(glm::vec3(x, y, z)) * Wiwa::Math::RandomRange(0.0f, emitter->m_p_initialPositionSphRadius);

			}
			break;
			default:
			{
				initPosition = emitter->m_p_initialPosition/*+ t3d->position*/;

			}
			break;
		}

		//initial rotation
		if (emitter->m_p_rangedInitialRotation)
		{
			float x = Wiwa::Math::RandomRange(emitter->m_p_minInitialRotation.x, emitter->m_p_maxInitialRotation.x);
			float y = Wiwa::Math::RandomRange(emitter->m_p_minInitialRotation.y, emitter->m_p_maxInitialRotation.y);
			float z = Wiwa::Math::RandomRange(emitter->m_p_minInitialRotation.z, emitter->m_p_maxInitialRotation.z);

			initRotation = emitter->m_p_initialRotation + followSpawnRot /*+ t3d->localRotation*/ + glm::vec3(x, y, z);
		}
		else
		{
			initRotation = emitter->m_p_initialRotation + followSpawnRot /*+ t3d->localRotation*/;
		}

		if (emitter->m_p_followEmitterRotationSpawn)
		{
			glm::vec3 rotationRad = glm::radians(initRotation);
			glm::vec3 orginalPos = initPosition;

			orginalPos = glm::rotate(orginalPos, rotationRad.x, glm::vec3(1.0f, 0.0f, 0.0f));
			orginalPos = glm::rotate(orginalPos, rotationRad.y, glm::vec3(0.0f, 1.0f, 0.0f));
			orginalPos = glm::rotate(orginalPos, rotationRad.z, glm::vec3(0.0f, 0.0f, 1.0f));

			initPosition = orginalPos;
		}

		initPosition += followSpawnPos;
		

		//initial scale
		if (emitter->m_p_rangedInitialScale)
		{
			float x = 0;
			float y = 0;
			float z = 0;

			if (emitter->m_p_uniformStartSize)
			{
				x = y = z = Wiwa::Math::RandomRange(emitter->m_p_minUniformStartSizeVal, emitter->m_p_maxUniformStartSizeVal);
			}
			else
			{
				x = Wiwa::Math::RandomRange(emitter->m_p_minInitialScale.x, emitter->m_p_maxInitialScale.x);
				y = Wiwa::Math::RandomRange(emitter->m_p_minInitialScale.y, emitter->m_p_maxInitialScale.y);
				z = Wiwa::Math::RandomRange(emitter->m_p_minInitialScale.z, emitter->m_p_maxInitialScale.z);
			}

			initScale = t3d->localScale + glm::vec3(x, y, z);
		}
		else
		{
			if (emitter->m_p_uniformStartSize)
			{
				initScale = t3d->localScale + glm::vec3(emitter->m_p_uniformStartSizeVal);
			}
			else
			{
				initScale = emitter->m_p_initialScale + t3d->localScale;
			}
		}

		particle.startPosition = initPosition;
		particle.transform.localPosition = initPosition;
		particle.transform.localRotation = initRotation;
		particle.startRotation = t3d->rotation;
		particle.transform.localScale = initScale;


		//initial velocity
		if (emitter->m_p_rangedVelocity)
		{
			float x = Wiwa::Math::RandomRange(emitter->m_p_minVelocity.x, emitter->m_p_maxVelocity.x);
			float y = Wiwa::Math::RandomRange(emitter->m_p_minVelocity.y, emitter->m_p_maxVelocity.y);
			float z = Wiwa::Math::RandomRange(emitter->m_p_minVelocity.z, emitter->m_p_maxVelocity.z);

			particle.velocity =  glm::vec3(x, y, z);
		}
		else
		{
			particle.velocity = emitter->m_p_initialVelocity;

			
		}

		if (emitter->m_p_followEmitterRotationSpawn)
		{
			glm::vec3 rotatedVel = particle.velocity;
			glm::vec3 rotationVelRad = glm::radians(emitter->m_p_initialVelocity);

			rotatedVel = glm::rotate(rotatedVel, rotationVelRad.x, glm::vec3(1.0f, 0.0f, 0.0f));
			rotatedVel = glm::rotate(rotatedVel, rotationVelRad.y, glm::vec3(0.0f, 1.0f, 0.0f));
			rotatedVel = glm::rotate(rotatedVel, rotationVelRad.z, glm::vec3(0.0f, 0.0f, 1.0f));

			particle.velocity = rotatedVel;

		}

		//initial angular velocity
		if (emitter->m_p_rangedAngularVelocity)
		{
			float x = Wiwa::Math::RandomRange(emitter->m_p_minInitialAngularVelocity.x, emitter->m_p_maxInitialAngularVelocity.x);
			float y = Wiwa::Math::RandomRange(emitter->m_p_minInitialAngularVelocity.y, emitter->m_p_maxInitialAngularVelocity.y);
			float z = Wiwa::Math::RandomRange(emitter->m_p_minInitialAngularVelocity.z, emitter->m_p_maxInitialAngularVelocity.z);

			particle.angularVelocity = glm::vec3(x, y, z);
		}
		else
		{
			particle.angularVelocity = emitter->m_p_initialAngularVelocity;
		}

		//initial growth velocity
		if (emitter->m_p_rangedGrowthVelocity)
		{

			float x = 0;
			float y = 0;
			float z = 0;

			particle.uniformGrowthVal = Wiwa::Math::RandomRange(emitter->m_p_minUniformGrowthVal, emitter->m_p_maxUniformGrowthVal);

			if (emitter->m_p_growUniformly)
			{
				x = y = z = particle.uniformGrowthVal;
			}
			else
			{
				x = Wiwa::Math::RandomRange(emitter->m_p_minInitialGrowthVelocity.x, emitter->m_p_maxInitialGrowthVelocity.x);
				y = Wiwa::Math::RandomRange(emitter->m_p_minInitialGrowthVelocity.y, emitter->m_p_maxInitialGrowthVelocity.y);
				z = Wiwa::Math::RandomRange(emitter->m_p_minInitialGrowthVelocity.z, emitter->m_p_maxInitialGrowthVelocity.z);
			}

			particle.growthVelocity = glm::vec3(x, y, z);
		}
		else
		{
			particle.uniformGrowthVal = emitter->m_p_uniformGrowthVal;

			if (emitter->m_p_growUniformly)
			{
				particle.growthVelocity.x = particle.growthVelocity.y = particle.growthVelocity.z = particle.uniformGrowthVal;
			}
			else
			{
				particle.growthVelocity = emitter->m_p_initialGrowthVelocity;
			}
		}

		//particle life time
		if (emitter->m_p_rangedLifeTime)
		{
			SetParticleLifeTime(particle, Wiwa::Math::RandomRange(emitter->m_p_minLifeTime, emitter->m_p_maxLifeTime));
		}
		else
		{
			SetParticleLifeTime(particle, emitter->m_p_lifeTime);
		}

		//particle.life_time = emitter->m_p_lifeTime;
		m_AvailableParticles--;		
	}

	void ParticleSystem::SpawnParticleSet()
	{
		ParticleEmitterComponent* emitter = GetComponent<ParticleEmitterComponent>();
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}

		int spawnAmount = 0;

		if (emitter->m_rangedSpawnAmount)
		{
			spawnAmount = Wiwa::Math::RandomRange(emitter->m_minSpawnAmount, emitter->m_maxSpawnAmount);
		}
		else
		{
			spawnAmount = emitter->m_spawnAmount;
		}

		for (unsigned int i = 0; i < (uint)spawnAmount; ++i)
		{
			if (m_AvailableParticles > 0)
			{
				int unusedParticle = FirstUnusedParticle();
				SpawnParticle(m_Particles[unusedParticle]);
			}
		}

		
	}

	unsigned int ParticleSystem::FirstUnusedParticle()
	{
		// search from last used particle, this will usually return almost instantly
		for (unsigned int i = m_LastUsedParticle; i < m_MaxParticles; ++i) {
			if (m_Particles[i].life_time <= 0.0f) {
				m_LastUsedParticle = i;
				return i;
			}
		}
		// otherwise, do a linear search
		for (unsigned int i = 0; i < m_LastUsedParticle; ++i) {
			if (m_Particles[i].life_time <= 0.0f) {
				m_LastUsedParticle = i;
				return i;
			}
		}
		// override first particle if all others are alive
		m_LastUsedParticle = 0;
		return 0;
	}

	void ParticleSystem::SetEmitterBools(ParticleEmitterComponent* emitter)
	{
		if (emitter == nullptr)
		{
			WI_CORE_ERROR("No ParticleEmitterComponent detected in Entity: {0}", (int)m_EntityId);
			return;
		}
		FixBool(emitter->m_meshChanged);
		FixBool(emitter->m_materialChanged);
		FixBool(emitter->m_loopSpawning);
		FixBool(emitter->m_rangedSpawnAmount);
		FixBool(emitter->m_p_rangedSpawnRate);
		FixBool(emitter->m_startActive);
		FixBool(emitter->m_active);
		FixBool(emitter->m_activeTimeChanged);
		FixBool(emitter->m_rangedTimeActive);
		FixBool(emitter->m_activeOverTime);
		FixBool(emitter->m_p_rangedLifeTime);
		FixBool(emitter->m_billboardActive);
		FixBool(emitter->m_p_followEmitterPosition);
		FixBool(emitter->m_p_followEmitterPositionSpawn);
		FixBool(emitter->m_p_positionTowardsPoint);
		FixBool(emitter->m_p_rangedVelocity);
		FixBool(emitter->m_p_useGravity);
		FixBool(emitter->m_p_followEmitterRotation);
		FixBool(emitter->m_p_followEmitterRotationSpawn);
		FixBool(emitter->m_p_rangedInitialRotation);
		FixBool(emitter->m_p_rangedAngularVelocity);
		FixBool(emitter->m_p_rangedInitialScale);
		FixBool(emitter->m_p_scaleOverTime);
		FixBool(emitter->m_p_rangedGrowthVelocity);
		FixBool(emitter->m_useAdditiveBlending);
		FixBool(emitter->m_p_positionFollowsRotation);
		FixBool(emitter->m_deactivateFaceCulling);
		FixBool(emitter->m_p_rangedSpawnDelay);
		FixBool(emitter->m_stopSizeAtZeroX);
		FixBool(emitter->m_stopSizeAtZeroY);
		FixBool(emitter->m_stopSizeAtZeroZ);
		FixBool(emitter->m_billboardLockAxisX, true);
		FixBool(emitter->m_billboardLockAxisY, true);
		FixBool(emitter->m_billboardLockAxisZ, true);
		FixBool(emitter->m_p_growUniformly);
		FixBool(emitter->m_destroyOnFinishActive);
		FixBool(emitter->m_p_uniformStartSize);
		FixBool(emitter->m_useMultiplyBlending);
		FixBool(emitter->m_destroyActiveParticles);
		FixBool(emitter->m_permanentParticles);
		FixBool(emitter->m_cycleLifeTime);
		FixBool(emitter->m_cycleColors);
		FixBool(emitter->m_useNewColorNodes);
	}

	void ParticleSystem::FixBool(bool& _bool, bool value)
	{
		if ((int)_bool < 0 || (int)_bool > 1)
			_bool = value;
	}

	

	void ParticleSystemHolder::OnUpdate()
	{

		if (Time::IsPlaying())
		{
			EntityManager& eman = Wiwa::SceneManager::getActiveScene()->GetEntityManager();

			std::vector<SystemHash>* children = eman.GetEntityChildren(m_EntityId);

			if (children->empty())
				eman.DestroyEntity(m_EntityId);
		}
		
	}

}
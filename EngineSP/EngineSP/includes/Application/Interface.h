#pragma once
#include "Context.h"

namespace SP {
	struct AppInterface {

		SP_INLINE virtual ~AppInterface() = default;

		// create entity
		template <typename Entt, typename... Args>
		SP_INLINE Entt CreateEntt(Args&&... args)
		{
			SP_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
			return std::move(Entt(&m_Context->Scene, std::forward<Args>(args)...));
		}

		// convert id to entity
		template<typename Entt>
		SP_INLINE Entt ToEntt(EntityID entity)
		{
			SP_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
			return std::move(Entt(&m_Context->Scene, entity));
		}

		// loop through entities
		template<typename Entt, typename Comp, typename Task>
		SP_INLINE void EnttView(Task&& task)
		{
			SP_STATIC_ASSERT(std::is_base_of<Entity, Entt>::value);
			m_Context->Scene.view<Comp>().each([this, &task]
			(auto entity, auto& comp)
				{
					task(std::move(Entt(&m_Context->Scene, entity)), comp);
				});
		}

		//Retrieving Layers
		template<typename Layer>
		SP_INLINE Layer* GetLayer() {
			SP_STATIC_ASSERT(std::is_base_of<AppInterface, Layer>::value);
			auto itr = std::find_if(m_Context->Layers.begin(), m_Context->Layers.end(), [this](auto layer) {
				return (layer->m_LayerID == TypeID<Layer>());
				});
			if (itr != m_Context->Layers.end())
				return static_cast<Layer*>(*itr);
			return nullptr;
		}

		// attach event callback
		template <typename Event, typename Callback>
		SP_INLINE void AttachCallback(Callback&& callback)
		{
			m_Context->Dispatcher.AttachCallback<Event>(std::move(callback), m_LayerID);
		}

		// post event
		template <typename Event, typename... Args>
		SP_INLINE void PostEvent(Args&&...args)
		{
			m_Context->Dispatcher.PostEvent<Event>(std::forward<Args>(args)...);
		}

		// post task event
		template <typename Task>
		SP_INLINE void PostTask(Task&& task)
		{
			m_Context->Dispatcher.PostTask(std::move(task));
		}

		// detach callback
		template <typename Event>
		SP_INLINE void DetachCallback()
		{
			m_Context->Dispatcher.DetachCallback<Event>(m_LayerID);
		}
		//Attaching Layers
		template<typename Layer, typename... Args>
		SP_INLINE Layer* AttachLayer(Args&&... args)
		{
			// check layer's type compliance 
			SP_STATIC_ASSERT(std::is_base_of<AppInterface, Layer>::value);
			// check if layer allready exist
			if (GetLayer<Layer>() != nullptr)
			{
#ifdef ENABLE_SPDLOG
				spdlog::error("Layer allready attached!");
#endif
				return nullptr;
			}
			// create layer and add to collection
			auto layer = new Layer(std::forward<Args>(args)...);
			m_Context->Layers.push_back(layer);
			layer->m_LayerID = TypeID<Layer>();
			layer->m_Context = m_Context;
			layer->OnStart();
			return layer;
		}

		//Detaching Layers
		template<typename Layer>
		SP_INLINE void DetachLayer()
		{
			SP_STATIC_ASSERT(std::is_base_of<AppInterface, Layer>::value);
			m_Context->Dispatcher.PostTask([this]
				{
					m_Context->Layers.erase(std::remove_if(m_Context->Layers.begin(),
						m_Context->Layers.end(), [this](auto& layer)
						{
							if (layer->m_LayerID == TypeID<Layer>())
							{
								m_Context->Dispatcher.EraseListener(layer->m_LayerID);
								SP_DELETE(layer);
								return true;
							}
							return false;
						}),
						m_Context->Layers.end());
				});
		}

	protected:
		SP_INLINE virtual void OnUpdate() {};
		SP_INLINE virtual void OnStart() {};

	private: 
		friend struct Application; //Allows the application to create/modify/destroy context
		AppContext* m_Context;
		uint32_t m_LayerID;
	};
}

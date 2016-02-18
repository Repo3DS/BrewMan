#ifndef BREWMAN_ASSETMANAGER_HPP
#define BREWMAN_ASSETMANAGER_HPP

#include <memory>
#include <map>
#include <assert.h>

namespace BrewMan {

template <class T>
class AssetManager {
public:
	static T& get(const std::string& filename)
	{
		static AssetManager<T> manager; // Yep, singleton

		auto item = manager.m_assets.find(filename);
		if (item == manager.m_assets.end())
		{
			std::unique_ptr<T> asset(new T);
			assert(asset->loadFromFile(filename));
			manager.m_assets.insert(std::make_pair(filename, std::move(asset)));
			return get(filename);
		}
		return *item->second;
	}

private:
	AssetManager<T>() {} // Empty constructor

	std::map<std::string, std::unique_ptr<T>> m_assets;
};

} // namespace BrewMan

#endif // BREWMAN_ASSETMANAGER_HPP

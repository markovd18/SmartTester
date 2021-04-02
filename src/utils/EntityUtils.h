//
// Author: markovd@students.zcu.cz
//

#ifndef SMARTTESTER_ENTITYUTILS_H
#define SMARTTESTER_ENTITYUTILS_H

namespace tester {
    template<typename T, typename... Args>
    HRESULT constructEntity(CDynamic_Library& library, const std::string& symbolName, Args... args) {
        if (!library.Is_Loaded()) {
            Logger::getInstance().error(L"Library is not loaded! Cannot construct entity.");
            return E_FAIL;
        }

        Logger::getInstance().debug(L"Creating entity with " + Widen_String(symbolName) + L" factory.");

        auto entityFactory = library.Resolve<T>(symbolName.c_str());
        if (!entityFactory) {
            Logger::getInstance().error(L"Error while loading " + Widen_String(symbolName) + L" factory!");
            return E_FAIL;
        }

        HRESULT creationResult = entityFactory(args...);

        if (!Succeeded(creationResult)) {
            return E_FAIL;
        }

        return S_OK;
    }

    template<typename T, typename D>
    HRESULT getEntityDescriptors(CDynamic_Library& library, const std::string& symbolName, D **begin, D **end) {
        if (!library.Is_Loaded()) {
            Logger::getInstance().error(L"Library is not loaded! Cannot get entity descriptors.");
            return E_FAIL;
        }

        auto descriptorsCreator = library.Resolve<T>(symbolName.c_str());
        if (!descriptorsCreator) {
            Logger::getInstance().error(L"Error while loading " + Widen_String(symbolName) + L" descriptors factory!");
            return E_FAIL;
        }

        return descriptorsCreator(begin, end);
    }

    template<typename T, typename D>
    D* getEntityDescriptor(CDynamic_Library& library, const GUID& guid, const std::string& symbolName) {
        D *begin, *end;

        HRESULT result = getEntityDescriptors<T>(library, symbolName, &begin, &end);
        if (!Succeeded(result)) {
            Logger::getInstance().error(L"Error while acquiring entity descriptors!");
            return nullptr;
        }

        while (begin != end) {
            if (begin->id == guid) {
                return begin;
            }
            begin++;
        }

        return nullptr;
    }
}

#endif //SMARTTESTER_ENTITYUTILS_H

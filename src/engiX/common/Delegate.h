#pragma once

#include <list>
#include <cassert>
#include <process.h>
#include <windows.h>
#include <set>
#include "engiXDefs.h"

namespace engiX
{
    // Used to cast Win32 CreateThread threastart to CRT _beginthreadex threadstart
    typedef unsigned(__stdcall *PfnCrtThreadStartEx)(void*);

    /// <summary>
    /// Represents a delegate to a member function that returns void and takes no parameters
    /// </summary>
    class IDelegate
    {
    public:
        virtual ~IDelegate() {}
        virtual bool Equals(const IDelegate* pOther) const = 0;
        virtual void operator()() = 0;
        virtual void Call() = 0;
    };

    /// <summary>
    /// Represents a delegate to a member function that returns void and takes 1 parameter which is TParam
    /// </summary>
    template<class TParam>
    class IDelegate1P
    {
    public:
        virtual ~IDelegate1P() {}
        virtual bool Equals(const IDelegate1P* pOther) const = 0;
        virtual void operator()(TParam param) = 0;
        virtual void Call(TParam param) = 0;
    };

    /// <summary>
    /// Represents a 0 parameter delegate to a method inside TReciever template class
    /// </summary>
    template<class TReciever>
    class Delegate : public IDelegate
    {
    public:
        typedef void (TReciever::*Callback)();

        Delegate(TReciever* pObj, Callback pfnCallback) :
            m_pObj(pObj), m_pfnCallback(pfnCallback) {}

        bool Equals(const IDelegate* pOther) const
        {
            const Delegate<TReciever>* other = static_cast<const Delegate<TReciever>*>(pOther);
            _ASSERTE(pOther);
            return other->m_pObj == m_pObj && other->m_pfnCallback == m_pfnCallback;
        }

        void operator()()
        {
            _ASSERTE(m_pObj);
            _ASSERTE(m_pfnCallback);
            (m_pObj->*m_pfnCallback)();
        }

        void Call()
        {
            _ASSERTE(m_pObj);
            _ASSERTE(m_pfnCallback);
            (m_pObj->*m_pfnCallback)();
        }

        void CallAsync()
        {
            HANDLE hThread = (HANDLE)_beginthreadex(nullptr, 0, (PfnCrtThreadStartEx)CallAsyncThreadStart_Static, this, 0, nullptr);
            _ASSERTE(hThread);
            (void)CloseHandle(hThread);
        }

        static DWORD WINAPI CallAsyncThreadStart_Static(_In_  LPVOID lpParameter)
        {
            IDelegate* pDelegate = reinterpret_cast<IDelegate*>(lpParameter);

            _ASSERTE(pDelegate)
            pDelegate->Call();

            return 0;
        }

    private:
        Callback m_pfnCallback;
        TReciever *m_pObj;
    };

    /// <summary>
    /// Represents a 1 parameter delegate to a method inside TReciever template class
    /// </summary>
    template<class TReciever, class TParam>
    class Delegate1P : public IDelegate1P<TParam>
    {
    public:
        typedef void (TReciever::*Callback)(TParam param);

        Delegate1P(TReciever* pObj, Callback pfnCallback) :
            m_pObj(pObj), m_pfnCallback(pfnCallback) {}

        bool Equals(const IDelegate1P* pOther) const
        {
            const Delegate1P<TReciever, TParam>* other = static_cast<const Delegate1P<TReciever, TParam>*>(pOther);
            _ASSERTE(pOther);
            return other->m_pObj == m_pObj && other->m_pfnCallback == m_pfnCallback;
        }

        virtual void operator()(TParam param)
        {
            _ASSERTE(m_pObj);
            _ASSERTE(m_pfnCallback);
            (m_pObj->*m_pfnCallback)(param);
        }

        virtual void Call(TParam param)
        {
            _ASSERTE(m_pObj);
            _ASSERTE(m_pfnCallback);
            (m_pObj->*m_pfnCallback)(param);
        }

    private:
        Callback m_pfnCallback;
        TReciever *m_pObj;
    };

    template<class TParam, class TReceiver>
    std::shared_ptr<IDelegate1P<TParam>> MakeDelegateP1(TReceiver* pR, typename Delegate1P<TReceiver, TParam>::Callback pFunc)
    {
        return std::shared_ptr<IDelegate1P<TParam>>(eNEW Delegate1P<TReceiver, TParam>(pR, pFunc));
    }

    /// <summary>
    /// This generic base class encapsulates the basic functionalities for MulticastDelegates
    /// A concrete MulticastDelegate class should be inheriting from MulticastDelegateBase and pass to its class template parameter the appropriate delegate class to use
    /// As well as implement Fire(*) and call operator (*) appropriately
    /// </summary>
    template<class TDelegate>
    class MulticastDelegateBase
    {
    public:
        typedef std::set<std::shared_ptr<TDelegate>> ObserverList;

        virtual MulticastDelegateBase::~MulticastDelegateBase()
        {
            m_observers.clear();
        }

        /// <summary>Register a delegate to be called when the MulticastDelegate is fired</summary>
        /// <param name="pCallback">The delegate to register</param>
        /// <returns>true on successful register, false otherwise</returns>
        ///
        bool operator += (std::shared_ptr<TDelegate> pCallback) { return Register(pCallback); }

        /// <summary>Unregister a delegate</summary>
        /// <param name="pCallback">The delegate to unregister</param>
        /// <returns>true on successful unregister, false otherwise</returns>
        ///
        bool operator -= (std::shared_ptr<TDelegate> callpCallbackback) { return Unregister(pCallback); }

        /// <summary>Fire the MulticastDelegate by calling all registered delegates</summary>

        bool Register(std::shared_ptr<TDelegate> pCallback)
        {
            return m_observers.insert(pCallback).second;
        }

        bool Unregister(std::shared_ptr<TDelegate> pCallback)
        {
            return m_observers.erase(pCallback) == 1;
        }

    protected:
        // Disallow MulticastDelegate instantiation
        MulticastDelegateBase() {}
        ObserverList m_observers;
    };

    /// <summary>
    /// Represents an MulticastDelegate to which many delegates can be registered to be called when the MulticastDelegate is fired
    /// </summary>
    class MulticastDelegate : public MulticastDelegateBase<IDelegate>
    {
    public:
        /// <summary>Fire the MulticastDelegate by calling all registered delegates</summary>
        /// <param name="pParam">A parameter to be passed to all delegates when called</param>
        ///
        void operator () () { Fire(); }

        void MulticastDelegate::Fire()
        {
            for(auto handler : m_observers)
                handler->Call();
        }
    };

    /// <summary>
    /// Represents an MulticastDelegate to which many delegates can be registered to be called when the MulticastDelegate is fired
    /// </summary>
    template<class TParam>
    class MulticastDelegate1P : public MulticastDelegateBase< IDelegate1P<TParam> >
    {
    public:
        /// <param name="pParam">A parameter to be passed to all delegates when called</param>
        ///
        void operator () (TParam param) { Fire(param); }

        void Fire(TParam param)
        {
            for(auto handler : m_observers)
                handler->Call(param);
        }
    };
}

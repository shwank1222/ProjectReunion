#pragma once

#include "CoreMinimal.h"

#include "Dom/JsonObject.h"
#include "HAL/IConsoleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Guid.h"
#include "Misc/Parse.h"
#include "RLSharedStringTable.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace RLDebug
{
    inline bool& InitializedRef()
    {
        static bool bInitialized = false;
        return bInitialized;
    }

    inline bool& EnabledRef()
    {
        static bool bEnabled = false;
        return bEnabled;
    }

    inline FString& RunIdRef()
    {
        static thread_local FString kRunId;
        return kRunId;
    }

    inline bool ReadDebugFlagFromUPlugin( bool& bOutDebugEnabled )
    {
        const TSharedPtr<IPlugin> spPlugin = IPluginManager::Get().FindPlugin( TEXT( MAIN_PLUGIN_NAME ) );
        if ( !spPlugin.IsValid() )
        {
            return false;
        }

        FString strJsonText;
        if ( !FFileHelper::LoadFileToString( strJsonText, *spPlugin->GetDescriptorFileName() ) )
        {
            return false;
        }

        TSharedPtr<FJsonObject> spRootObject;
        const TSharedRef<TJsonReader<>> spReader = TJsonReaderFactory<>::Create( strJsonText );
        if ( !FJsonSerializer::Deserialize( spReader, spRootObject ) || !spRootObject.IsValid() )
        {
            return false;
        }

        return spRootObject->TryGetBoolField( TEXT( "IsRLDebugEnable" ), bOutDebugEnabled );
    }

    inline void Initialize()
    {
        if ( InitializedRef() )
        {
            return;
        }

        bool bEnabled = false;
        bool bEnabledFromUPlugin = false;
        if ( ReadDebugFlagFromUPlugin( bEnabledFromUPlugin ) )
        {
            bEnabled = bEnabledFromUPlugin;
        }

        static TAutoConsoleVariable<int32> CVarRLDebugImport(
            TEXT( "rl.DebugImport" ),
            -1,
            TEXT( "RL import debug logging switch. -1: use RLPlugin.uplugin IsRLDebugEnable, 0: off, 1: on." ),
            ECVF_Default );
        const int32 nCVarValue = CVarRLDebugImport.GetValueOnAnyThread();
        if ( nCVarValue >= 0 )
        {
            bEnabled = ( nCVarValue != 0 );
        }

        int32 nCmdValue = 0;
        if ( FParse::Value( FCommandLine::Get(), TEXT( "RLDebug=" ), nCmdValue ) )
        {
            bEnabled = ( nCmdValue != 0 );
        }

        EnabledRef() = bEnabled;
        InitializedRef() = true;
    }

    inline bool IsEnabled()
    {
        if ( !InitializedRef() )
        {
            Initialize();
        }
        return EnabledRef();
    }

    inline FString NewRunId()
    {
        const FString strGuid = FGuid::NewGuid().ToString( EGuidFormats::Digits ).Left( 8 );
        return FString::Printf( TEXT( "%lld-%s" ), FDateTime::UtcNow().GetTicks(), *strGuid );
    }

    inline void SetRunId( const FString& strRunId )
    {
        RunIdRef() = strRunId;
    }

    inline FString EnsureRunId()
    {
        FString& strRunId = RunIdRef();
        if ( strRunId.IsEmpty() )
        {
            strRunId = NewRunId();
        }
        return strRunId;
    }

    class FImportRunScope
    {
    public:
        explicit FImportRunScope( const FString& strRunId )
            : m_strPreviousRunId( RunIdRef() )
        {
            SetRunId( strRunId );
        }

        ~FImportRunScope()
        {
            SetRunId( m_strPreviousRunId );
        }

    private:
        FString m_strPreviousRunId;
    };

    inline void LogPhaseStart( const FLogCategoryBase& kCategory,
                               const FString& strRunId,
                               const TCHAR* strPhase,
                               const FString& strContext )
    {
        if ( !IsEnabled() )
        {
            return;
        }
        FMsg::Logf(
            __FILE__,
            __LINE__,
            kCategory.GetCategoryName(),
            ELogVerbosity::Log,
            TEXT( "[RLDBG][Phase=%s][Event=Start] %s [RunId=%s]" ),
            strPhase ? strPhase : TEXT( "Unknown" ),
            strContext.IsEmpty() ? TEXT( "-" ) : *strContext,
            *strRunId );
    }

    inline void LogPhaseEnd( const FLogCategoryBase& kCategory,
                             const FString& strRunId,
                             const TCHAR* strPhase,
                             const double dElapsedMs,
                             const bool bSuccess,
                             const FString& strContext )
    {
        if ( !IsEnabled() )
        {
            return;
        }
        FMsg::Logf(
            __FILE__,
            __LINE__,
            kCategory.GetCategoryName(),
            ELogVerbosity::Log,
            TEXT( "[RLDBG][Phase=%s][Event=End][Success=%d][ElapsedSec=%.3f] %s [RunId=%s]" ),
            strPhase ? strPhase : TEXT( "Unknown" ),
            bSuccess ? 1 : 0,
            dElapsedMs / 1000.0,
            strContext.IsEmpty() ? TEXT( "-" ) : *strContext,
            *strRunId );
    }

    inline void LogPhaseError( const FLogCategoryBase& kCategory,
                               const FString& strRunId,
                               const TCHAR* strPhase,
                               const FString& strError )
    {
        if ( !IsEnabled() )
        {
            return;
        }
        FMsg::Logf(
            __FILE__,
            __LINE__,
            kCategory.GetCategoryName(),
            ELogVerbosity::Warning,
            TEXT( "[RLDBG][Phase=%s][Event=Error] %s [RunId=%s]" ),
            strPhase ? strPhase : TEXT( "Unknown" ),
            strError.IsEmpty() ? TEXT( "-" ) : *strError,
            *strRunId );
    }

    class FPhaseScope
    {
    public:
        FPhaseScope( const FLogCategoryBase& kCategory,
                     const FString& strRunId,
                     const TCHAR* strPhase,
                     const FString& strContext = TEXT( "" ) )
            : m_kCategory( kCategory )
            , m_strRunId( strRunId )
            , m_strPhase( strPhase ? strPhase : TEXT( "Unknown" ) )
            , m_strContext( strContext )
            , m_dStartSeconds( FPlatformTime::Seconds() )
        {
            LogPhaseStart( m_kCategory, m_strRunId, *m_strPhase, m_strContext );
        }

        ~FPhaseScope()
        {
            const double dElapsedMs = ( FPlatformTime::Seconds() - m_dStartSeconds ) * 1000.0;
            LogPhaseEnd( m_kCategory, m_strRunId, *m_strPhase, dElapsedMs, m_bSuccess, m_strContext );
        }

        void Fail( const FString& strError )
        {
            m_bSuccess = false;
            LogPhaseError( m_kCategory, m_strRunId, *m_strPhase, strError );
        }

    private:
        const FLogCategoryBase& m_kCategory;
        FString m_strRunId;
        FString m_strPhase;
        FString m_strContext;
        double m_dStartSeconds = 0.0;
        bool m_bSuccess = true;
    };
}

<%!
import re
from templates import helper as th
%><%
    n=namespace
    N=n.upper()

    x=tags['$x']
    X=x.upper()
%>/*
 *
 * Copyright (C) 2019-2024 Intel Corporation
 *
 * Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM Exceptions.
 * See LICENSE.TXT
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 * @file ${name}.cpp
 *
 */
#include "${x}_mock.hpp"
#include "${x}_mock_helpers.hpp"

namespace driver
{
    %for obj in th.get_adapter_functions(specs):
    ///////////////////////////////////////////////////////////////////////////////
    <%
        fname = th.make_func_name(n, tags, obj)
    %>/// @brief Intercept function for ${fname}
    %if 'condition' in obj:
    #if ${th.subt(n, tags, obj['condition'])}
    %endif
    __${x}dlllocal ${x}_result_t ${X}_APICALL
    ${fname}(
        %for line in th.make_param_lines(n, tags, obj):
        ${line}
        %endfor
        )
    try {
        ${x}_result_t result = ${X}_RESULT_SUCCESS;
        ${th.get_initial_null_set(obj)}

        ${th.make_pfncb_param_type(n, tags, obj)} params = { &${",&".join(th.make_param_lines(n, tags, obj, format=["name"]))} };

        auto beforeCallback = reinterpret_cast<ur_mock_callback_t>(
                mock::callbacks.get_before_callback("${fname}"));
        if(beforeCallback) {
            result = beforeCallback( &params );
            if(result != UR_RESULT_SUCCESS) {
                return result;
            }
        }

        auto replaceCallback = reinterpret_cast<ur_mock_callback_t>(
                mock::callbacks.get_replace_callback("${fname}"));
        if(replaceCallback) {
            result = replaceCallback( &params );
        }
        else
        {
            <%
                # We can use the loader epilogue to know when we should be creating mock handles
                epilogue = th.get_loader_epilogue(specs, n, tags, obj, meta)
            %>
            %if 'NativeHandle' in fname:
                <% func_class = th.subt(namespace, tags, obj['class'], False, True) %>
                %if 'CreateWith' in fname:
                    *ph${func_class} = reinterpret_cast<ur_${func_class.lower()}_handle_t>(hNative${func_class});
                    mock::retainDummyHandle(*ph${func_class});
                %else:
                    *phNative${func_class} = reinterpret_cast<ur_native_handle_t>(h${func_class});
                %endif
            %else:
                %if fname == 'urAdapterGet' or fname == 'urDeviceGet' or fname == 'urPlatformGet':
                    <%
                        num_param = th.find_param_name(".*pNum.*", n, tags, obj)
                    %>
                    if(${num_param}) {
                        *${num_param} = 1;
                    }
                %endif
                %for item in epilogue:
                    %if item['release']:
                        mock::releaseDummyHandle(${item['name']});
                    %elif item['retain']:
                        mock::retainDummyHandle(${item['name']});
                    %elif 'type' in item:
                        %if 'range' in item or ('optional' in item and item['optional']):
                        // optional output handle
                        if(${item['name']}) {
                            *${item['name']} = mock::createDummyHandle<${item['type']}>();
                        }
                        %else:
                        *${item['name']} = mock::createDummyHandle<${item['type']}>();
                        %endif
                    %endif
                %endfor
            %endif
            result = UR_RESULT_SUCCESS;
        }

        if(result != UR_RESULT_SUCCESS) {
            return result;
        }

        auto afterCallback = reinterpret_cast<ur_mock_callback_t>(
                mock::callbacks.get_after_callback("${fname}"));
        if(afterCallback) {
            return afterCallback( &params );
        }
        
        return result;
    } catch(...) { return exceptionToResult(std::current_exception()); }
    %if 'condition' in obj:
    #endif // ${th.subt(n, tags, obj['condition'])}
    %endif

    %endfor
} // namespace driver

#if defined(__cplusplus)
extern "C" {
#endif

%for tbl in th.get_pfntables(specs, meta, n, tags):
///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's ${tbl['name']} table
///        with current process' addresses
///
/// @returns
///     - ::${X}_RESULT_SUCCESS
///     - ::${X}_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::${X}_RESULT_ERROR_UNSUPPORTED_VERSION
${X}_DLLEXPORT ${x}_result_t ${X}_APICALL
${tbl['export']['name']}(
    %for line in th.make_param_lines(n, tags, tbl['export']):
    ${line}
    %endfor
    )
try {
    if( nullptr == pDdiTable )
        return ${X}_RESULT_ERROR_INVALID_NULL_POINTER;

    if( driver::d_context.version < version )
        return ${X}_RESULT_ERROR_UNSUPPORTED_VERSION;

    ${x}_result_t result = ${X}_RESULT_SUCCESS;

    %for obj in tbl['functions']:
    %if 'condition' in obj:
#if ${th.subt(n, tags, obj['condition'])}
    %endif
    pDdiTable->${th.append_ws(th.make_pfn_name(n, tags, obj), 41)} = driver::${th.make_func_name(n, tags, obj)};
    %if 'condition' in obj:
#else
    pDdiTable->${th.append_ws(th.make_pfn_name(n, tags, obj), 41)} = nullptr;
#endif
    %endif

    %endfor
    return result;
} catch(...) { return exceptionToResult(std::current_exception()); }

%endfor
#if defined(__cplusplus)
}
#endif
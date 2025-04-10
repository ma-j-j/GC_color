bool BaseCamera_Daheng::takePhoto(
    const QVector< bool > &image_valid_array,
    result_process_fun_t new_result_process_fun,
    void *user_data ) noexcept {
    auto data_ptr = m_d.data_ptr();
    if( bool _Expected = false;
        !m_d->m_under_callback_flag.compare_exchange_strong( _Expected, true ) ) {
        return false;
    }
    auto &&guard1 = qScopeGuard( [ & ]() noexcept {
        m_d->m_under_callback_flag.store( false, std::memory_order_release );
    } );
    if( !data_ptr->m_isCameraOpened ) {
        data_ptr->m_error_process_fun( "Camera is not opened" );
        return false;
    }
    else if( image_valid_array.size() != 2 ) {
        data_ptr->m_error_process_fun( "InValid image_valid_array" );
        return false;
    }
    else if( data_ptr->m_mode.mode != EmExecuteMode::emTakePhoto ) {
        data_ptr->m_error_process_fun( "InValid Camera Mode" );
        return false;
    }
    data_ptr->m_takphoto_mode.m_result_process_fun = { new_result_process_fun, user_data };
    data_ptr->m_takphoto_mode.m_image_valid_array = image_valid_array;
    try {
        data_ptr->m_objFeatureControlPtr->GetCommandFeature( "TriggerSoftware" )->Execute();
    }
    catch( std::exception &e ) {
        data_ptr->m_error_process_fun( e.what() );
        return false;
    }
    catch( ... ) {
        data_ptr->m_error_process_fun( "unkown error" );
        return false;
    }
    guard1.dismiss();
    return true;
}
